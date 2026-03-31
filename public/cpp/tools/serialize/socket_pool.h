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
// File overview: Optimized socket buffer pool with thread-local cache and statistics.

#pragma once

#include "tools/tools/tools_time_wheel.h"
#include "tools/tools/tools_thread.h"
#include "tools/tools/tools_time.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <format>
#include <atomic>
#include <thread>
#include <array>
#include <list>
#include <new>

namespace ngl
{
	// Socket buffer pool statistics
	struct pool_stats
	{
		std::atomic<uint64_t> m_alloc_count{0};		// Total allocations
		std::atomic<uint64_t> m_free_count{0};		// Total frees
		std::atomic<uint64_t> m_cache_hit{0};		// Cache hits
		std::atomic<uint64_t> m_cache_miss{0};		// Cache misses
		std::atomic<uint64_t> m_large_alloc{0};		// Large allocations (>max bucket)
		std::atomic<uint64_t> m_alloc_fail{0};		// Allocation failures

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
		struct block_head
		{
			uint32_t m_magic = 0;
			int32_t m_index = -1;
		};

		// Thread-local cache for lock-free fast path
		struct thread_cache
		{
			std::array<std::list<char*>, TCOUNT> m_local_pool;
			std::array<int32_t, TCOUNT> m_local_count{};
		};

		std::array<std::list<char*>, TCOUNT>	m_pool;
		std::array<int32_t, TCOUNT>				m_bytes;
		const std::array<int32_t, TCOUNT>		m_counts;
		std::shared_mutex						m_mutex;
		pool_stats								m_stats;

		// Thread-local storage
		thread_local static thread_cache* tl_cache;
		std::unordered_map<std::thread::id, std::unique_ptr<thread_cache>> m_thread_caches;
		std::shared_mutex m_cache_mutex;

		enum
		{
			enum_block_magic = 0x534F434B,  // 'SOCK'
			enum_head_bytes = sizeof(block_head),
			enum_tl_cache_max = 8,  // Max items per bucket in thread-local cache
		};

		static void write_head(char* abuff, int32_t aindex)
		{
			block_head lhead{.m_magic = enum_block_magic, .m_index = aindex};
			memcpy(abuff, &lhead, sizeof(lhead));
		}

		static bool read_head(const char* abuff, block_head& ahead)
		{
			if (abuff == nullptr) return false;
			memcpy(&ahead, abuff, sizeof(ahead));
			return ahead.m_magic == enum_block_magic;
		}

		char* nmalloc(int32_t aindex, int32_t abytes = 0)
		{
			int32_t lbytes = (aindex >= 0 && aindex < TCOUNT)
				? m_bytes[aindex] + enum_head_bytes
				: abytes + enum_head_bytes;

			if (lbytes <= 0) return nullptr;

			char* lpbuff = new(std::nothrow) char[lbytes];
			if (lpbuff == nullptr)
			{
				m_stats.m_alloc_fail.fetch_add(1, std::memory_order_relaxed);
				return nullptr;
			}
			write_head(lpbuff, aindex);
			return &lpbuff[enum_head_bytes];
		}

		void nfree(char* abuff, bool areturn_to_pool = true)
		{
			if (abuff == nullptr) return;

			char* lbuff = abuff - enum_head_bytes;
			block_head lhead;
			if (!read_head(lbuff, lhead))
			{
				std::cout << std::format("socket_buff_pool<{},{}>::nfree invalid head\n",
					TINITBYTES, TCOUNT);
				return;
			}

			const int32_t lindex = lhead.m_index;
			if (lindex >= 0 && lindex < TCOUNT && areturn_to_pool)
			{
				lock_write(m_mutex);
				m_pool[lindex].push_back(abuff);
			}
			else
			{
				delete[] lbuff;
			}
		}

		thread_cache* get_thread_cache()
		{
			if (tl_cache == nullptr)
			{
				auto tid = std::this_thread::get_id();
				lock_write(m_cache_mutex);
				auto& cache_ptr = m_thread_caches[tid];
				if (!cache_ptr)
				{
					cache_ptr = std::make_unique<thread_cache>();
				}
				tl_cache = cache_ptr.get();
			}
			return tl_cache;
		}

	public:
		buff_pool(const std::array<int32_t, TCOUNT>& acounts) : m_counts(acounts)
		{
			static_assert(TCOUNT > 0, "socket_buff_pool requires at least one bucket");
			m_bytes[0] = TINITBYTES;
			for (int32_t i = 0; i < TCOUNT; ++i)
			{
				if (i != 0) m_bytes[i] = m_bytes[i - 1] * 2;

				for (int j = 0; j < m_counts[i]; ++j)
				{
					char* lbuff = nmalloc(i);
					if (lbuff != nullptr) m_pool[i].push_back(lbuff);
				}
			}

			tools::wheel_parm lparm
			{
				.m_ms = 10 * tools::time::MILLISECOND,
				.m_intervalms = [](int64_t) { return 10 * tools::time::MILLISECOND; },
				.m_count = 0x7fffffff,
				.m_fun = [this](const tools::wheel_node*) { time_free(); },
			};
			tools::twheel::wheel().addtimer(lparm);
		}

		char* malloc_private(int32_t abytes)
		{
			if (abytes <= 0) return nullptr;

			m_stats.m_alloc_count.fetch_add(1, std::memory_order_relaxed);

			auto itor = std::lower_bound(m_bytes.begin(), m_bytes.end(), abytes);
			if (itor == m_bytes.end())
			{
				m_stats.m_large_alloc.fetch_add(1, std::memory_order_relaxed);
				return nmalloc(-1, abytes);
			}

			int32_t lpos = static_cast<int32_t>(itor - m_bytes.begin());

			// Try thread-local cache first
			thread_cache* cache = get_thread_cache();
			if (cache && !cache->m_local_pool[lpos].empty())
			{
				char* ret = cache->m_local_pool[lpos].front();
				cache->m_local_pool[lpos].pop_front();
				cache->m_local_count[lpos]--;
				m_stats.m_cache_hit.fetch_add(1, std::memory_order_relaxed);
				return ret;
			}

			m_stats.m_cache_miss.fetch_add(1, std::memory_order_relaxed);

			// Try global pool
			{
				lock_write(m_mutex);
				if (!m_pool[lpos].empty())
				{
					char* ret = m_pool[lpos].front();
					m_pool[lpos].pop_front();
					return ret;
				}
			}

			return nmalloc(lpos);
		}

		void free_private(char* abuff)
		{
			if (abuff == nullptr) return;

			m_stats.m_free_count.fetch_add(1, std::memory_order_relaxed);

			char* lbuff = abuff - enum_head_bytes;
			block_head lhead;
			if (!read_head(lbuff, lhead))
			{
				std::cout << std::format("socket_buff_pool<{},{}>::free_private invalid head\n",
					TINITBYTES, TCOUNT);
				return;
			}

			const int32_t lindex = lhead.m_index;
			if (lindex < 0 || lindex >= TCOUNT)
			{
				delete[] lbuff;
				return;
			}

			// Return to thread-local cache if not full
			thread_cache* cache = get_thread_cache();
			if (cache && cache->m_local_count[lindex] < enum_tl_cache_max)
			{
				cache->m_local_pool[lindex].push_back(abuff);
				cache->m_local_count[lindex]++;
				return;
			}

			// Return to global pool
			nfree(abuff);
		}

		void time_free()
		{
			lock_write(m_mutex);
			for (int32_t i = 0; i < TCOUNT; ++i)
			{
				int32_t lcount = m_counts[i] * 2;
				std::list<char*>& ls = m_pool[i];
				while (ls.size() > static_cast<size_t>(lcount))
				{
					nfree(ls.front(), false);
					ls.pop_front();
				}
			}
		}

		const pool_stats& stats() const { return m_stats; }
		void print_stats() const { m_stats.print(); }
	};

	template <int TINITBYTES, int TCOUNT>
	thread_local typename buff_pool<TINITBYTES, TCOUNT>::thread_cache*
		buff_pool<TINITBYTES, TCOUNT>::tl_cache = nullptr;

	// Optimized bucket sizes for typical socket scenarios
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
				// Bucket counts: 64, 128, 256, 512, 1K, 2K, 4K, 8K, 16K, 32K, 64K, 128K
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
	};

}// namespace ngl
