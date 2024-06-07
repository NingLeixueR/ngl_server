#pragma once

#include <functional>
#include <set>

#include "threadtools.h"
#include "time_wheel.h"
#include "type.h"

namespace ngl
{
	template <typename T, int NUM>
	class cache_list
	{
		cache_list(const cache_list&) = delete;
		cache_list& operator=(const cache_list&) = delete;

		cache_list() {}

		std::set<i64_actorid>						m_ls;
		std::function<void(std::set<i64_actorid>&)> m_fun;
		std::shared_mutex							m_mutex;
	public:
		static cache_list<T, NUM>& getInstance()
		{
			static cache_list<T, NUM> ltemp;
			return ltemp;
		}

		void set_cachefun(const std::function<void(std::set<i64_actorid>&)>& afun)
		{
			m_fun = afun;
			twheel::wheel().addtimer(wheel_parm
				{
					.m_ms = 1 * 1000,
					.m_intervalms = [](int64_t) {return 1 * 1000; } ,
					.m_count = 0x7fffffff,
					.m_pram = nullptr,
					.m_fun = std::bind(&cache_list<T,NUM>::execute, &getInstance(), std::placeholders::_1),
				});
		}

		void push(i64_actorid aid)
		{
			monopoly_shared_lock(m_mutex);
			m_ls.insert(aid);
		}

		void push(std::vector<i64_actorid>& aidlist)
		{
			monopoly_shared_lock(m_mutex);
			m_ls.insert(aidlist.begin(), aidlist.end());
		}

		void execute(wheel_node*)
		{
			monopoly_shared_lock(m_mutex);
			if (m_ls.empty() != true)
			{
				m_fun(m_ls);
				m_ls.clear();
			}
		}
	};
}// namespace ngl