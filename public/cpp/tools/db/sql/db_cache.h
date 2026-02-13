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
#include "type.h"

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
			twheel::wheel().addtimer(wheel_parm
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

		inline void execute(const wheel_node*)
		{
			{
				lock_write(m_mutex);
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