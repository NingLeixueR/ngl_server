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
#include <cassert>
#include <cstdint>
#include <cstring>
#include <format>
#include <atomic>
#include <vector>
#include <mutex>
#include <array>
#include <bit>
#include <new>

namespace ngl
{
	struct bucket_stats
	{
		std::atomic<uint64_t> m_current_allocated{0};  // Currently held externally (alloc - free for this bucket).
		std::atomic<uint64_t> m_peak_allocated{0};     // Historical peak of m_current_allocated.
		std::atomic<uint64_t> m_current_cached{0};     // Currently sitting in local_pool (updated on drain).
	};

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
			uint64_t hit = m_cache_hit.load(std::memory_order_relaxed);
			uint64_t miss = m_cache_miss.load(std::memory_order_relaxed);
			uint64_t total = hit + miss;
			double hit_rate = total > 0 ? (100.0 * hit / total) : 0.0;
			std::cout << std::format(
				"SocketPool Stats: alloc={} free={} hit_rate={:.2f}% large={} fail={}\n",
				m_alloc_count.load(std::memory_order_relaxed),
				m_free_count.load(std::memory_order_relaxed),
				hit_rate,
				m_large_alloc.load(std::memory_order_relaxed),
				m_alloc_fail.load(std::memory_order_relaxed)
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
			std::atomic<bool>   m_has_pending{false};
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
				int16_t expected = s_slot_next.load(std::memory_order_relaxed);
				while (expected < enum_max_threads)
				{
					if (s_slot_next.compare_exchange_weak(expected,
						static_cast<int16_t>(expected + 1),
						std::memory_order_relaxed, std::memory_order_relaxed))
					{
						m_slot = expected;
						s_slots[m_slot].store(&m_cache, std::memory_order_release);
						return;
					}
				}
			}
		};

		thread_local static tl_holder tl_data;

		static constexpr std::array<int32_t, TCOUNT> make_bytes()
		{
			std::array<int32_t, TCOUNT> bytes{};
			bytes[0] = TINITBYTES;
			for (int32_t i = 1; i < TCOUNT; ++i)
				bytes[i] = bytes[i - 1] * 2;
			return bytes;
		}

		static constexpr std::array<int32_t, TCOUNT> m_bytes = make_bytes();
		const std::array<int32_t, TCOUNT> m_counts;
		pool_stats                        m_stats;
		std::array<bucket_stats, TCOUNT>  m_bucket_stats;

		// ---- internal helpers ----

		static void write_head(char* abuff, int16_t aindex, int16_t aslot)
		{
			block_head h{.m_magic = enum_block_magic, .m_index = aindex, .m_slot = aslot};
			memcpy(abuff, &h, sizeof(h));
		}

		static block_head* read_head(const char* abuff)
		{
			if (abuff == nullptr)
				return nullptr;
			auto lphead = (block_head*)(abuff - enum_head_bytes);
			if (lphead->m_magic != enum_block_magic)
				return nullptr;
			return lphead;
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
			if (!acache.m_has_pending.load(std::memory_order_relaxed))
				return;

			std::vector<char*> ltmp;
			{
				std::lock_guard<std::mutex> lk(acache.m_pending_mutex);
				ltmp.swap(acache.m_pending);
				acache.m_has_pending.store(false, std::memory_order_relaxed);
			}

			for (char* p : ltmp)
			{
				block_head* h = read_head(p);
				if (h == nullptr || h->m_index < 0 || h->m_index >= TCOUNT)
				{
					delete[](p - enum_head_bytes);
					continue;
				}
				acache.m_local_pool[h->m_index].push_back(p);
			}

			// Update cached count and trim: if cached exceeds 3x current_allocated, trim to 2x.
			for (int32_t i = 0; i < TCOUNT; ++i)
			{
				auto& bucket = acache.m_local_pool[i];
				m_bucket_stats[i].m_current_cached.store(bucket.size(), std::memory_order_relaxed);

				uint64_t allocated = m_bucket_stats[i].m_current_allocated.load(std::memory_order_relaxed);
				size_t trigger = static_cast<size_t>(allocated) * 3;
				if (bucket.size() > trigger && allocated > 0)
				{
					size_t target = static_cast<size_t>(allocated) * 2;
					while (bucket.size() > target)
					{
						delete[](bucket.back() - enum_head_bytes);
						bucket.pop_back();
					}
					m_bucket_stats[i].m_current_cached.store(bucket.size(), std::memory_order_relaxed);
				}
			}
		}

	public:
		buff_pool(const std::array<int32_t, TCOUNT>& acounts) : m_counts(acounts)
		{
			static_assert(TCOUNT > 0, "buff_pool requires at least one bucket");
			static_assert((TINITBYTES & (TINITBYTES - 1)) == 0, "TINITBYTES must be power of 2");
		}

		void track_alloc(int16_t aindex)
		{
			if (aindex >= 0 && aindex < TCOUNT)
			{
				uint64_t cur = m_bucket_stats[aindex].m_current_allocated.fetch_add(1, std::memory_order_relaxed) + 1;
				uint64_t peak = m_bucket_stats[aindex].m_peak_allocated.load(std::memory_order_relaxed);
				while (cur > peak)
				{
					if (m_bucket_stats[aindex].m_peak_allocated.compare_exchange_weak(peak, cur, std::memory_order_relaxed))
						break;
				}
			}
		}

		void track_free(int16_t aindex)
		{
			if (aindex >= 0 && aindex < TCOUNT)
			{
				m_bucket_stats[aindex].m_current_allocated.fetch_sub(1, std::memory_order_relaxed);
			}
		}

		char* malloc_private(int32_t abytes)
		{
			if (abytes <= 0)
				return nullptr;

			m_stats.m_alloc_count.fetch_add(1, std::memory_order_relaxed);

			int16_t lpos;
			if (abytes <= TINITBYTES)
			{
				lpos = 0;
			}
			else
			{
				constexpr int base_shift = std::countr_zero(static_cast<uint32_t>(TINITBYTES));
				int shift = std::bit_width(static_cast<uint32_t>(abytes - 1));
				lpos = static_cast<int16_t>(shift - base_shift);
				if (lpos >= TCOUNT)
				{
					m_stats.m_large_alloc.fetch_add(1, std::memory_order_relaxed);
					auto& tl = ensure_tl();
					return nmalloc(-1, tl.m_slot, abytes);
				}
			}
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
				track_alloc(lpos);
				return ret;
			}

			// 3) Allocate fresh.
			m_stats.m_cache_miss.fetch_add(1, std::memory_order_relaxed);
			char* ret = nmalloc(lpos, tl.m_slot);
			if (ret != nullptr)
				track_alloc(lpos);
			return ret;
		}

		void free_private(char* abuff)
		{
			if (abuff == nullptr)
				return;

			m_stats.m_free_count.fetch_add(1, std::memory_order_relaxed);

			char* lraw = abuff - enum_head_bytes;
			block_head* h = read_head(abuff);
			if (h == nullptr)
			{
				assert(false && "buff_pool::free_private: invalid block_head magic");
				std::cout << std::format(
					"buff_pool<{},{}>::free_private invalid head, ptr={}\n",
					TINITBYTES, TCOUNT, static_cast<void*>(lraw));
				return;
			}

			int16_t lindex = h->m_index;
			int16_t lslot = h->m_slot;

			// Oversized: just delete.
			if (lindex < 0 || lindex >= TCOUNT)
			{
				delete[] lraw;
				return;
			}

			// Track free for bucket stats.
			track_free(lindex);

			auto& tl = ensure_tl();

			// Same-thread fast path (only for registered slots).
			if (lslot >= 0 && lslot == tl.m_slot)
			{
				tl.m_cache.m_local_pool[lindex].push_back(abuff);
				return;
			}

			// Cross-thread: push into owner's pending queue.
			if (lslot >= 0 && lslot < enum_max_threads)
			{
				thread_cache* lcache = s_slots[lslot].load(std::memory_order_acquire);
				if (lcache != nullptr)
				{
					std::lock_guard<std::mutex> lk(lcache->m_pending_mutex);
					lcache->m_pending.push_back(abuff);
					lcache->m_has_pending.store(true, std::memory_order_relaxed);
					return;
				}
			}

			// Owner dead or slot invalid: delete directly.
			delete[] lraw;
		}

		const pool_stats& stats() const { return m_stats; }
		const std::array<bucket_stats, TCOUNT>& get_bucket_stats() const { return m_bucket_stats; }
		static constexpr const std::array<int32_t, TCOUNT>& get_bytes() { return m_bytes; }
		const std::array<int32_t, TCOUNT>& get_counts() const { return m_counts; }
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
				{100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100}
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

		static const std::array<bucket_stats, enum_socket_pool_count>& get_bucket_stats()
		{
			return instance().buff_pool::get_bucket_stats();
		}

		static const std::array<int32_t, enum_socket_pool_count>& get_bucket_bytes()
		{
			return buff_pool<enum_socket_pool_init_bytes, enum_socket_pool_count>::get_bytes();
		}

		static const std::array<int32_t, enum_socket_pool_count>& get_bucket_counts()
		{
			return instance().get_counts();
		}
	};
}// namespace ngl
