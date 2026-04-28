/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
*
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
*
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Thread-local socket buffer pool with cross-thread pending queue.
//
// Design:
//   - Each thread owns a thread_cache (local pool per bucket).
//   - block_head stores int16 thread slot so free() finds the owner in O(1).
//   - Same-thread free goes directly into local pool (zero contention).
//   - Cross-thread free pushes into the owner's mutex-guarded pending queue.
//   - malloc drains pending (lock + swap), trims buckets > 2x count, then allocs.
//   - No global pool, no shared_mutex, no timer-based reclamation.

#pragma once

#include <algorithm>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <format>
#include <atomic>
#include <vector>
#include <mutex>
#include <array>
#include <new>

namespace ngl
{
	struct pool_stats
	{
		std::atomic<uint64_t> m_alloc_count{0};
		std::atomic<uint64_t> m_free_count{0};
		std::atomic<uint64_t> m_cache_hit{0};
		std::atomic<uint64_t> m_cache_miss{0};
		std::atomic<uint64_t> m_large_alloc{0};
		std::atomic<uint64_t> m_alloc_fail{0};

		void print() const
		{
			uint64_t total = m_cache_hit.load() + m_cache_miss.load();
			double hit_rate = total > 0 ? (100.0 * m_cache_hit.load() / total) : 0.0;
			std::cout << std::format(
				"SocketPool Stats: alloc={} free={} hit_rate={:.2f}% large={} fail={}\n",
				m_alloc_count.load(), m_free_count.load(), hit_rate,
				m_large_alloc.load(), m_alloc_fail.load()
			);
		}
	};

	template <int TINITBYTES, int TCOUNT>
	class buff_pool
	{
		// Prepended before every user buffer.
		struct block_head
		{
			uint32_t m_magic = 0;
			int16_t  m_index = -1;		// bucket index, -1 for oversized
			int16_t  m_slot  = -1;		// owning thread slot
		};

		enum
		{
			enum_block_magic = 0x534F434B,
			enum_max_threads = 128,
			enum_head_bytes = sizeof(block_head),
		};

		// Per-thread: local buckets + pending queue for cross-thread returns.
		// No destructor: the pool is process-lifetime. Thread-local caches are
		// intentionally leaked on thread exit — the OS reclaims all memory when
		// the process terminates. This also avoids destructor-order races between
		// tl_holder, thread_cache, and cross-thread free() callers.
		struct thread_cache
		{
			std::array<std::vector<char*>, TCOUNT> m_local_pool;
			std::vector<char*>  m_pending;
			std::mutex          m_pending_mutex;
		};

		// Global slot table: O(1) lookup by int16 index.
		static inline std::array<std::atomic<thread_cache*>, enum_max_threads> s_slots{};
		static inline std::atomic<int16_t> s_slot_next{0};

		// Thread-local holder: owns the cache and registers/unregisters the slot.
		struct tl_holder
		{
			thread_cache m_cache;
			int16_t      m_slot = -1;

			void init()
			{
				if (m_slot >= 0)
					return;
				m_slot = s_slot_next.fetch_add(1, std::memory_order_relaxed);
				if (m_slot < enum_max_threads)
					s_slots[m_slot].store(&m_cache, std::memory_order_release);
			}
		};

		thread_local static tl_holder tl_data;

		std::array<int32_t, TCOUNT>       m_bytes;
		const std::array<int32_t, TCOUNT> m_counts;
		pool_stats                        m_stats;

		// ---- internal helpers ----

		static void write_head(char* abuff, int16_t aindex, int16_t aslot)
		{
			block_head h{.m_magic = enum_block_magic, .m_index = aindex, .m_slot = aslot};
			memcpy(abuff, &h, sizeof(h));
		}

		static bool read_head(const char* abuff, block_head& ahead)
		{
			if (abuff == nullptr)
				return false;
			memcpy(&ahead, abuff, sizeof(ahead));
			return ahead.m_magic == enum_block_magic;
		}

		tl_holder& ensure_tl()
		{
			tl_data.init();
			return tl_data;
		}

		char* nmalloc(int16_t aindex, int16_t aslot, int32_t abytes = 0)
		{
			int32_t lbytes = (aindex >= 0 && aindex < TCOUNT)
				? m_bytes[aindex] + enum_head_bytes
				: abytes + enum_head_bytes;

			if (lbytes <= 0)
				return nullptr;

			char* lraw = new(std::nothrow) char[lbytes];
			if (lraw == nullptr)
			{
				m_stats.m_alloc_fail.fetch_add(1, std::memory_order_relaxed);
				return nullptr;
			}
			write_head(lraw, aindex, aslot);
			return &lraw[enum_head_bytes];
		}

		// Drain pending queue into local pool, then trim oversized buckets.
		void drain_pending(thread_cache& acache)
		{
			std::vector<char*> ltmp;
			{
				std::lock_guard<std::mutex> lk(acache.m_pending_mutex);
				ltmp.swap(acache.m_pending);
			}

			for (char* p : ltmp)
			{
				block_head h;
				if (!read_head(p - enum_head_bytes, h) || h.m_index < 0 || h.m_index >= TCOUNT)
				{
					delete[](p - enum_head_bytes);
					continue;
				}
				acache.m_local_pool[h.m_index].push_back(p);
			}

			// Trim: if any bucket exceeds 2x configured count, delete the excess.
			for (int32_t i = 0; i < TCOUNT; ++i)
			{
				auto& bucket = acache.m_local_pool[i];
				size_t limit = static_cast<size_t>(m_counts[i]) * 2;
				while (bucket.size() > limit)
				{
					delete[](bucket.back() - enum_head_bytes);
					bucket.pop_back();
				}
			}
		}

	public:
		buff_pool(const std::array<int32_t, TCOUNT>& acounts) : m_counts(acounts)
		{
			static_assert(TCOUNT > 0, "buff_pool requires at least one bucket");
			m_bytes[0] = TINITBYTES;
			for (int32_t i = 1; i < TCOUNT; ++i)
				m_bytes[i] = m_bytes[i - 1] * 2;
		}

		char* malloc_private(int32_t abytes)
		{
			if (abytes <= 0)
				return nullptr;

			m_stats.m_alloc_count.fetch_add(1, std::memory_order_relaxed);

			auto itor = std::lower_bound(m_bytes.begin(), m_bytes.end(), abytes);
			if (itor == m_bytes.end())
			{
				m_stats.m_large_alloc.fetch_add(1, std::memory_order_relaxed);
				auto& tl = ensure_tl();
				return nmalloc(-1, tl.m_slot, abytes);
			}

			int16_t lpos = static_cast<int16_t>(itor - m_bytes.begin());
			auto& tl = ensure_tl();
			auto& cache = tl.m_cache;

			// 1) Drain cross-thread pending returns.
			drain_pending(cache);

			// 2) Try local pool.
			if (!cache.m_local_pool[lpos].empty())
			{
				char* ret = cache.m_local_pool[lpos].back();
				cache.m_local_pool[lpos].pop_back();
				m_stats.m_cache_hit.fetch_add(1, std::memory_order_relaxed);
				return ret;
			}

			// 3) Allocate fresh.
			m_stats.m_cache_miss.fetch_add(1, std::memory_order_relaxed);
			return nmalloc(lpos, tl.m_slot);
		}

		void free_private(char* abuff)
		{
			if (abuff == nullptr)
				return;

			m_stats.m_free_count.fetch_add(1, std::memory_order_relaxed);

			char* lraw = abuff - enum_head_bytes;
			block_head h;
			if (!read_head(lraw, h))
			{
				std::cout << std::format(
					"buff_pool<{},{}>::free_private invalid head\n",
					TINITBYTES, TCOUNT);
				return;
			}

			// Oversized: just delete.
			if (h.m_index < 0 || h.m_index >= TCOUNT)
			{
				delete[] lraw;
				return;
			}

			auto& tl = ensure_tl();

			// Same-thread fast path.
			if (h.m_slot == tl.m_slot)
			{
				tl.m_cache.m_local_pool[h.m_index].push_back(abuff);
				return;
			}

			// Cross-thread: push into owner's pending queue.
			if (h.m_slot >= 0 && h.m_slot < enum_max_threads)
			{
				thread_cache* owner = s_slots[h.m_slot].load(std::memory_order_acquire);
				if (owner != nullptr)
				{
					std::lock_guard<std::mutex> lk(owner->m_pending_mutex);
					owner->m_pending.push_back(abuff);
					return;
				}
			}

			// Owner dead or slot invalid: delete directly.
			delete[] lraw;
		}

		const pool_stats& stats() const { return m_stats; }
		void print_stats() const { m_stats.print(); }
	};

	template <int TINITBYTES, int TCOUNT>
	thread_local typename buff_pool<TINITBYTES, TCOUNT>::tl_holder
		buff_pool<TINITBYTES, TCOUNT>::tl_data{};

	enum
	{
		enum_socket_pool_count = 12,
		enum_socket_pool_init_bytes = 64,
	};

	class socket_pool :
		public buff_pool<enum_socket_pool_init_bytes, enum_socket_pool_count>
	{
		socket_pool() :
			buff_pool<enum_socket_pool_init_bytes, enum_socket_pool_count>(
				{100, 80, 60, 50, 40, 30, 25, 20, 15, 10, 5, 3}
			)
		{}

		static socket_pool& instance()
		{
			static socket_pool lpool;
			return lpool;
		}

	public:
		static char* malloc(int abytes)
		{
			return instance().malloc_private(abytes);
		}

		static void free(char* abuff)
		{
			instance().free_private(abuff);
		}

		static void print_stats()
		{
			instance().stats().print();
		}

		static const pool_stats& get_stats()
		{
			return instance().stats();
		}
	};
}// namespace ngl
