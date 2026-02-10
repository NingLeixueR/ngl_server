/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "threadtools.h"
#include "time_wheel.h"
#include "nlog.h"

#include <algorithm>
#include <array>
#include <list>

#define ISUSENETPOOL	(true)

namespace ngl
{
	template <int TINITBYTES, int TCOUNT>
	class netbuff
	{
		std::array<std::list<char*>, TCOUNT> m_pool;
		std::array<int32_t, TCOUNT>			 m_bytes;
		const std::array<int32_t, TCOUNT>	 m_counts;
		std::shared_mutex					 m_mutex;

		enum
		{
			enum_int32char = sizeof(int32_t) + sizeof(char),
		};

		char* nmalloc(int32_t aindex, int32_t abytes = 0)
		{
			int32_t lbytes = 0;
			if (aindex >= TCOUNT || aindex < 0)
			{
				lbytes = abytes + enum_int32char;
			}
			else
			{
				lbytes = m_bytes[aindex] + enum_int32char;
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
			lpbuff[0] = 'k';
			*(int32_t*)(&lpbuff[1]) = aindex;
			return &lpbuff[enum_int32char];
		}

		void nfree(char* abuff, bool abool = true)
		{
			if (abuff == nullptr)
			{
				return;
			}
			char* lbuff = abuff - enum_int32char;
			if (*lbuff != 'k')
			{
				log_error()->print("netbuff<{},{}>::nfree != k", TINITBYTES, TCOUNT);
				return;
			}
			int lindex = *(int*)(abuff - sizeof(int));
			if (lindex >= 0 && lindex < TCOUNT)
			{
				if (abool)
				{
					monopoly_shared_lock(m_mutex);
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
					m_pool[i].push_back(lbuff);
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
			auto itor = std::lower_bound(m_bytes.begin(), m_bytes.end(), abytes);
			if (itor != m_bytes.end())
			{
				int32_t lpos = (int32_t)(itor - m_bytes.begin());
				{
					monopoly_shared_lock(m_mutex);
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

		// # 每分钟定时回收 超过初始化两倍的清空
		void time_free()
		{
			monopoly_shared_lock(m_mutex);
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