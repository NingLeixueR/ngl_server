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
// File overview: Implements logic for actor log.


#include "actor/actor_logic/actor_log/actor_log.h"

namespace ngl
{
	actor_log::actor_log(int32_t atype):
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_LOG, 
					.m_area = tab_self_area,
					.m_id = nconfig.tid()
				},
				.m_weight = 0x7fffffff
			})
	{
		for (int i = ELOG_DEFAULT;i< ELOG_COUNT;i = i<<1)
		{
			if ((atype & i) != 0)
			{
				logfile::config lconfig
				{
					.m_id = nconfig.nodeid(),
					.m_dir = "log",
					.m_type = (ELOG_TYPE)i,
					.m_flush_time = 10,
				};
				nconfig.info()->find("log_flushtime", lconfig.m_flush_time);
				m_logarr.emplace((ELOG_TYPE)i, logfile::create_make(lconfig));
			}
		}
	}

	ENUM_ACTOR actor_log::actor_type()
	{
		return ACTOR_LOG;
	}

	i64_actorid actor_log::actorid(i32_actordataid aid)
	{
		return nguid::make(actor_type(), tab_self_area, aid);
	}

	void actor_log::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		// flush
		int32_t lflushtime = m_logarr.begin()->second->m_config.flush() / tools::time::MILLISECOND;
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, lflushtime) == false)
		{
			log_error()->print("actor_log::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		auto lparmflush = std::make_shared<log_timerparm>(log_timerparm::e_logflush);
		timerparm_set_parm(&tparm, lparmflush);
		set_timer(tparm);

		// create
		np_timerparm tparmcreate;
		if (make_timerparm::parm_day(tparmcreate, 0, 0, 0) == false)
		{
			log_error()->print("actor_log::init() make_timerparm::make_timerparm::day(tparmcreate, 0, 0, 0) == false!!!");
			return;
		}
		auto lparmcreate = std::make_shared<log_timerparm>(log_timerparm::e_create);
		timerparm_set_parm(&tparmcreate, lparmcreate);
		set_timer(tparmcreate);
	}

	void actor_log::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_log::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_log::nregister()
	{
		// Timer.
		actor::register_timer<actor_log>(&actor_log::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_log
			, np_logitem
		>();
	}
	
	bool actor_log::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		std::shared_ptr<log_timerparm> lparm = std::static_pointer_cast<log_timerparm>(adata.get_data()->m_parm);
		if (lparm->m_type == log_timerparm::e_logflush)
		{
			for (const auto& lpair : m_logarr)
			{
				lpair.second->flush();
			}
			return true;
		}
		if (lparm->m_type == log_timerparm::e_create)
		{
			for (const auto& lpair : m_logarr)
			{
				lpair.second->close_fstream();
				lpair.second->create();
			}
			return true;
		}
		return true;
	}
}//namespace ngl
