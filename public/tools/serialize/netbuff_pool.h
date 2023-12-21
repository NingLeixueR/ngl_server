#pragma once

#include <array>
#include <list>
#include "threadtools.h"
#include "nlog.h"
#include "time_wheel.h"

namespace ngl
{
	template <int InitBytes, int Count>
	class netbuff
	{
		std::array<std::list<char*>, Count> m_pool;
		std::array<int32_t, Count>			m_bytes;
		const std::array<int32_t, Count>	m_counts;
		std::shared_mutex					m_mutex;

		char* nmalloc(int aindex, int abytes = 0)
		{
			int lbytes = 0;
			if (aindex >= Count || aindex < 0)
			{
				lbytes = abytes + sizeof(int) + sizeof(char);
			}
			else
			{
				lbytes = m_bytes[aindex] + sizeof(int) + sizeof(char);
			}
			if (lbytes <= 0)
				return nullptr;
			
			char* lpbuff = new(std::nothrow) char[lbytes];
			if (lpbuff == nullptr)
				return nullptr;
			lpbuff[0] = 'k';
			*(int*)(&lpbuff[1]) = aindex;
			return &lpbuff[sizeof(int) + sizeof(char)];
		}

		void nfree(char* abuff, bool abool = true)
		{
			if (abuff == nullptr)
				return;
			char* lbuff = abuff - (sizeof(int) + sizeof(char));
			if (*lbuff != 'k')
			{
				LogLocalError("netbuff<%,%>::free != k", InitBytes, Count);
				return;
			}
			int lindex = *(int*)(abuff - sizeof(int));
			if (lindex >= 0 && lindex < Count)
			{
				if (abool)
				{
					monopoly_shared_lock(m_mutex);
					m_pool[lindex].push_back(abuff);
				}
				else
					delete[] lbuff;
			}
			else
			{
				delete[] lbuff;
			}
		}
	public:
		netbuff(const std::array<int32_t, Count>& acounts):
			m_counts(acounts)
		{
			m_bytes[0] = InitBytes;
			for (int i = 0; i < Count; ++i)
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
				.m_ms = 10 * 1000,
				.m_intervalms = [](int64_t) {return 10 * 1000; } ,
				.m_count = 0x7fffffff,
				.m_fun = [this](wheel_node* anode) {time_free(); },
			};
			twheel::wheel().addtimer(lparm);
		}

		char* malloc_private(int abytes)
		{
			char* lpbuff = nullptr;
			for (int i = 0; i < Count; ++i)
			{
				if (abytes <= m_bytes[i])
				{
					{
						monopoly_shared_lock(m_mutex);
						if (m_pool[i].empty() == false)
						{
							char* ret = nullptr;
							ret = *m_pool[i].begin();
							m_pool[i].pop_front();
							return ret;
						}
					}
					return  nmalloc(i);
				}
			}
			return nmalloc(-1, abytes);
		}

		void free_private(char* abuff)
		{
			nfree(abuff);
		}

		// ### 每分钟定时回收 超过初始化两倍的清空
		void time_free()
		{
			monopoly_shared_lock(m_mutex);
			for (int i = 0; i < Count; ++i)
			{
				int lcount = m_counts[i] * 2;
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
		enum_pool_count = 10,
		enum_pool_buff_bytes = 64,
	};

	class netbuff_pool : public netbuff<enum_pool_buff_bytes, enum_pool_count>
	{
		netbuff_pool():
			netbuff<enum_pool_buff_bytes, enum_pool_count>({50, 50, 40, 30, 20, 20, 20, 10, 10, 10})
		{}
	public:
		static netbuff_pool& getInstance()
		{
			static netbuff_pool lnet;
			return lnet;
		}

		static char* malloc(int abytes)
		{
			return getInstance().malloc_private(abytes);
		}

		static void free(char* abuff)
		{
			getInstance().free_private(abuff);
		}
	};
}