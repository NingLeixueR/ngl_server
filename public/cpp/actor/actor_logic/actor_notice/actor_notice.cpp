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
// File overview: Implements logic for actor notice.


#include "actor/actor_logic/actor_notice/actor_notice.h"

namespace ngl
{
	actor_notice::actor_notice() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_NOTICE,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_notice::actor_type()
	{
		return ACTOR_NOTICE;
	}

	i64_actorid actor_notice::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_notice::init()
	{
		// Bind the DB structure: DB.set(this);
		m_notice.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_notice::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_notice::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_notice::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_notice::nregister()
	{
		// Timer.
		actor::register_timer<actor_notice>(&actor_notice::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_notice
			, mforward<np_gm>
			, np_actor_addnotice
			, mforward<pbnet::PROBUFF_NET_NOTICE>
		>(e_ready_all);
	}

	bool actor_notice::sendnotice(const std::string& anotice, int32_t astarttime, int32_t afinishtime)
	{
		auto pro = std::make_shared<np_actor_addnotice>();
		pro->m_notice = anotice;
		pro->m_starttime = astarttime;
		pro->m_finishtime = afinishtime;
		actor::send_actor(actorid(), actor_notice::actorid(), pro);
		return true;
	}

	bool actor_notice::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl
