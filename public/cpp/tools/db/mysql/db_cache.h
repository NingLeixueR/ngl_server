#pragma once

#include <functional>
#include <set>

#include "threadtools.h"
#include "time_wheel.h"
#include "type.h"

namespace ngl
{
	class db_cache
	{
		db_cache(const db_cache&) = delete;
		db_cache& operator=(const db_cache&) = delete;

		std::set<i64_actorid>						m_cachelist;
		std::set<i64_actorid>						m_copycachelist;
		std::function<void(std::set<i64_actorid>&)> m_fun;
		std::shared_mutex							m_mutex;
	public:
		inline db_cache() 
		{
		}

		inline void set_cachefun(const std::function<void(std::set<i64_actorid>&)>& afun, int32_t aintervalms)
		{
			m_fun = afun;
			twheel::wheel().addtimer(wheel_parm
				{
					.m_ms			= aintervalms,
					.m_intervalms	= [aintervalms](int64_t) {return aintervalms; } ,
					.m_count		= 0x7fffffff,
					.m_pram			= nullptr,
					.m_fun			= std::bind_front(&db_cache::execute, this),
				});
		}

		inline void push(i64_actorid aid)
		{
			monopoly_shared_lock(m_mutex);
			m_cachelist.insert(aid);
		}

		inline void push(const std::vector<i64_actorid>& aidlist)
		{
			monopoly_shared_lock(m_mutex);
			m_cachelist.insert(aidlist.begin(), aidlist.end());
		}

		inline void execute(const wheel_node*)
		{
			{
				monopoly_shared_lock(m_mutex);
				m_cachelist.swap(m_copycachelist);
			}
			if (!m_copycachelist.empty())
			{
				m_fun(m_copycachelist);
				m_copycachelist.clear();
			}
		}
	};
}// namespace ngl