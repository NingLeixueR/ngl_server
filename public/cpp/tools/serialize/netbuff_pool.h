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
// File overview: Declares interfaces for serialize.

#pragma once

#include "tools/localtime.h"
#include "tools/threadtools.h"
#include "tools/time_wheel.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <list>
#include <new>

#define ISUSENETPOOL	(true)

namespace ngl
{
	template <int TINITBYTES, int TCOUNT>
	class netbuff
	{
		struct block_head
		{
			uint32_t m_magic = 0;
			int32_t m_index = -1;
		};

		std::array<std::list<char*>, TCOUNT> m_pool;
		std::array<int32_t, TCOUNT>			 m_bytes;
		const std::array<int32_t, TCOUNT>	 m_counts;
		std::shared_mutex					 m_mutex;

		enum
		{
			enum_block_magic = 0x4E474C42,
			enum_head_bytes = sizeof(block_head),
		};

		static void write_head(char* abuff, int32_t aindex)
		{
			block_head lhead
			{
				.m_magic = enum_block_magic,
				.m_index = aindex,
			};
			memcpy(abuff, &lhead, sizeof(lhead));
		}

		static bool read_head(const char* abuff, block_head& ahead)
		{
			if (abuff == nullptr)
			{
				return false;
			}
			memcpy(&ahead, abuff, sizeof(ahead));
			return ahead.m_magic == enum_block_magic;
		}

		char* nmalloc(int32_t aindex, int32_t abytes = 0)
		{
			int32_t lbytes = 0;
			if (aindex >= TCOUNT || aindex < 0)
			{
				lbytes = abytes + enum_head_bytes;
			}
			else
			{
				lbytes = m_bytes[aindex] + enum_head_bytes;
			}
			if (lbytes <= 0)
			{
				return nullptr;
			}
			
			char* lpbuff = new(std::nothrow) char[lbytes];
			if (lpbuff == nullptr)
			{
				return nullptr;
			}
			write_head(lpbuff, aindex);
			return &lpbuff[enum_head_bytes];
		}

		void nfree(char* abuff, bool abool = true)
		{
			if (abuff == nullptr)
			{
				return;
			}
			char* lbuff = abuff - enum_head_bytes;
			block_head lhead;
			if (!read_head(lbuff, lhead))
			{
				std::cout << std::format("netbuff<{},{}>::nfree invalid head", TINITBYTES, TCOUNT) << std::endl;
				return;
			}
			const int32_t lindex = lhead.m_index;
			if (lindex >= 0 && lindex < TCOUNT)
			{
				if (abool)
				{
					lock_write(m_mutex);
					m_pool[lindex].push_back(abuff);
				}
				else
				{
					delete[] lbuff;
				}
			}
			else
			{
				delete[] lbuff;
			}
		}
	public:
		netbuff(const std::array<int32_t, TCOUNT>& acounts):
			m_counts(acounts)
		{
			static_assert(TCOUNT > 0, "netbuff requires at least one bucket");
			m_bytes[0] = TINITBYTES;
			for (int32_t i = 0; i < TCOUNT; ++i)
			{
				if (i != 0)
				{
					m_bytes[i] = m_bytes[i - 1] * 2;
				}
				for (int j = 0; j < m_counts[i]; ++j)
				{
					char* lbuff = nmalloc(i);
					if (lbuff != nullptr)
					{
						m_pool[i].push_back(lbuff);
					}
				}
			}

			wheel_parm lparm
			{
				.m_ms = 10 * localtime::MILLISECOND,
				.m_intervalms = [](int64_t) {return 10 * localtime::MILLISECOND; } ,
				.m_count = 0x7fffffff,
				.m_fun = [this](const wheel_node*)
				{
					time_free(); 
				},
			};
			twheel::wheel().addtimer(lparm);
		}

		char* malloc_private(int32_t abytes)
		{
			if (abytes <= 0)
			{
				return nullptr;
			}
			auto itor = std::lower_bound(m_bytes.begin(), m_bytes.end(), abytes);
			if (itor != m_bytes.end())
			{
				int32_t lpos = (int32_t)(itor - m_bytes.begin());
				{
					lock_write(m_mutex);
					if (m_pool[lpos].empty() == false)
					{
						char* ret = nullptr;
						ret = *m_pool[lpos].begin();
						m_pool[lpos].pop_front();
						return ret;
					}
				}
				return  nmalloc(lpos);
			}
			return nmalloc(-1, abytes);
		}

		void free_private(char* abuff)
		{
			nfree(abuff);
		}

		// # Initialize clear
		void time_free()
		{
			lock_write(m_mutex);
			for (int32_t i = 0; i < TCOUNT; ++i)
			{
				int32_t lcount = m_counts[i] * 2;
				std::list<char*>& ls = m_pool[i];
				for (; ls.size() > lcount;)
				{
					nfree(*ls.begin(), false);
					ls.pop_front();
				}
			}
		}
	};

	enum
	{
		enum_pool_count			= 10,
		enum_pool_buff_bytes	= 64,
	};

	class netbuff_pool : 
		public netbuff<enum_pool_buff_bytes, enum_pool_count>
	{
		netbuff_pool():
			netbuff<enum_pool_buff_bytes, enum_pool_count>({50, 50, 40, 30, 20, 20, 20, 10, 10, 10})
		{}
	public:
		static netbuff_pool& instance()
		{
			static netbuff_pool lnet;
			return lnet;
		}

		static char* malloc(int abytes)
		{
			if constexpr (ISUSENETPOOL)
			{
				return instance().malloc_private(abytes);
			}
			else
			{
				return new char[abytes];
			}
		}

		static void free(char* abuff)
		{
			if constexpr (ISUSENETPOOL)
			{
				instance().free_private(abuff);
			}
			else
			{
				delete[] abuff;
			}
		}
	};
}// namespace ngl
