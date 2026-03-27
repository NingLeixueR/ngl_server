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
// File overview: Declares interfaces for sql.

#pragma once

#include "tools/tools/tools_thread.h"
#include "tools/tools/tools_time_wheel.h"
#include "tools/type.h"

#include <functional>
#include <set>

namespace ngl
{
	class db_cache
	{
		db_cache(const db_cache&) = delete;
		db_cache& operator=(const db_cache&) = delete;

		using callback = std::function<void(std::set<i64_actorid>&)>;

		std::set<i64_actorid>	m_cachelist;
		std::set<i64_actorid>	m_copycachelist;
		callback				m_fun;
		std::shared_mutex		m_mutex;
	public:
		db_cache() = default;

		inline void set_cachefun(const callback& afun, int32_t aintervalms)
		{
			m_fun = afun;
			// Dirty ids are flushed in batches on the shared timer wheel so hot
			// paths only need to enqueue ids and return.
			tools::twheel::wheel().addtimer(tools::wheel_parm
				{
					.m_ms			= aintervalms,
					.m_intervalms	= [aintervalms](int64_t) {return aintervalms; } ,
					.m_count		= 0x7fffffff,
					.m_pram			= nullptr,
					.m_fun			= std::bind_front(&db_cache::execute, this),
				}
			);
		}

		inline void push(i64_actorid aid)
		{
			lock_write(m_mutex);
			m_cachelist.insert(aid);
		}

		inline void push(const std::vector<i64_actorid>& aidlist)
		{
			lock_write(m_mutex);
			m_cachelist.insert(aidlist.begin(), aidlist.end());
		}

		inline void execute(const tools::wheel_node*)
		{
			{
				lock_write(m_mutex);
				m_cachelist.swap(m_copycachelist);
			}
			if (!m_copycachelist.empty())
			{
				// Run the callback outside the lock to avoid blocking producers.
				m_fun(m_copycachelist);
				m_copycachelist.clear();
			}
		}
	};
}// namespace ngl
