#pragma once

#include <functional>
#include <set>

#include "threadtools.h"
#include "time_wheel.h"
#include "type.h"

namespace ngl
{
	template <typename T>
	class db_cache
	{
		db_cache(const db_cache&) = delete;
		db_cache& operator=(const db_cache&) = delete;

		std::set<i64_actorid>						m_ls;
		std::function<void(std::set<i64_actorid>&)> m_fun;
		std::shared_mutex							m_mutex;
	public:
		db_cache() 
		{
		}

		void set_cachefun(
			const std::function<void(std::set<i64_actorid>&)>& afun,
			int32_t aintervalms
		)
		{
			m_fun = afun;
			twheel::wheel().addtimer(wheel_parm
				{
					.m_ms = aintervalms,
					.m_intervalms = [aintervalms](int64_t) {return aintervalms; } ,
					.m_count = 0x7fffffff,
					.m_pram = nullptr,
					.m_fun =  std::bind_front(&db_cache<T>::execute, this),
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

		void execute(const wheel_node*)
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