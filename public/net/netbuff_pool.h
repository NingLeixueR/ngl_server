#pragma once

#include <list>
#include <map>
#include <array>
#include "threadtools.h"
#include "time_wheel.h"

namespace ngl
{
	template <int NBCOUNT>
	class nbuff
	{
		std::array<std::list<char*>, NBCOUNT> m_pool;
		std::array<int32_t, NBCOUNT> m_bytes;
		ngl::mutex m_mutex;
		int m_initcount;

		enum
		{
			enum_expand_bytes = sizeof(int) + sizeof(char),
		};

		char* malloc_expand(int aindex, int abytes)
		{
			char* lp = new(std::nothrow)char[abytes + enum_expand_bytes];
			if (lp == nullptr)
				return nullptr;
			lp[0] = 'k';
			memset(&lp[1], aindex, sizeof(int));
			return &lp[enum_expand_bytes];
		}

		bool index(char* abuff, int& aindex, char*& aretbuff)
		{
			aretbuff = abuff - enum_expand_bytes;
			if (*aretbuff != 'k')
				return false;
			aindex = *(int*)(abuff - sizeof(int));
			return aindex >= NBCOUNT;
		}
	public:
		nbuff(int ainitbate, int amultiple, int ainitcount):
			m_initcount(ainitcount)
		{
			int lvalues = ainitbate;
			for (int i = 0; i < NBCOUNT; ++i)
			{
				m_bytes[i] = lvalues * (i + 1);
				for (int j = 0; j < ainitcount; ++j)
				{
					m_pool[i].push_back(malloc_expand(i, m_bytes[i]));
				}
			}

			time_wheel_pram lparm
			{
				.m_ms = 60 * 1000,
				.m_intervalms = 60 * 1000,
				.m_count = 0x7fffffff,
				.m_fun = [](wheel_node* anode) {netbuff_pool::getInstance().time_free(); }
			};

			twheel::wheel().addtimer(lparm);
		}

	public:
		char* malloc_private(int abytes)
		{
			char* lpbuff = nullptr;
			for (int i = 0; i < NBCOUNT; ++i)
			{
				if (abytes <= m_bytes[i])
				{
					bool lempty;
					{
						monopoly_shared_lock(m_mutex);
						lempty = m_pool[i].empty();
					}
					if (lempty)
					{
						return malloc_expand(i, m_bytes[i]);
					}
					else
					{
						char* ret = nullptr;
						{
							monopoly_shared_lock(m_mutex);
							ret = *m_pool[i].begin();
							m_pool[i].pop_front();
						}
						return ret;
					}
					break;
				}
			}

			malloc_expand(-1, abytes);
			return lpbuff;
		}

		void free_private(char* abuff)
		{
			int32_t lindex = -1;
			char* lbuff = nullptr;
			if (index(abuff, lindex, lbuff))
			{
				if (lindex >= 0 && lindex < NBCOUNT)
				{
					monopoly_shared_lock(m_mutex);
					m_pool[lindex].push_back(abuff);
				}
				else
				{
					delete[] lbuff;
				}
			}
		}

		// ### 每分钟定时回收 超过初始化两倍的清空
		void time_free()
		{
			int lcount = m_initcount * 2;
			monopoly_shared_lock(m_mutex);
			for (int i = 0; i < enum_pool_count; ++i)
			{
				for (; m_pool[i].size() > lcount;)
				{
					int32_t lindex = -1;
					char* lbuff = nullptr;
					if (index(*m_pool[i].begin(), lindex, lbuff))
					{
						delete[] lbuff;
						m_pool[i].pop_front();
					}
				}
			}
		}
	};

	enum
	{
		enum_pool_count = 10,
		enum_pool_buff_bytes = 64,
		enum_pool_everyinit_count = 10,
	};

	class netbuff_pool : public nbuff<enum_pool_count>
	{
		

		netbuff_pool():
			nbuff<enum_pool_count>(enum_pool_buff_bytes, 2, enum_pool_everyinit_count)
		{
		}
	public:
		static netbuff_pool& getInstance()
		{
			static netbuff_pool lnet;
			return lnet;
		}
	public:
		static char* malloc(int abytes)
		{
			return getInstance().malloc_private(abytes);
		}

		static void free(char* abuff)
		{
			return getInstance().free_private(abuff);
		}
	};


}