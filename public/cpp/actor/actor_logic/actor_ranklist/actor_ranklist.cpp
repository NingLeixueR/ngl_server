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
// File overview: Implements logic for actor ranklist.


#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"

namespace ngl
{
	actor_ranklist::actor_ranklist() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_RANKLIST,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_ranklist::actor_type()
	{
		return ACTOR_RANKLIST;
	}

	i64_actorid actor_ranklist::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_ranklist::init()
	{
		// Bind the DB structure: DB.set(this);
		m_ranklist.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_ranklist::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
		
	}

	void actor_ranklist::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_ranklist>::instance(id_guid()).exit();
	}

	void actor_ranklist::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_ranklist::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_ranklist::nregister()
	{
		// Timer.
		actor::register_timer<actor_ranklist>(&actor_ranklist::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_ranklist
			, mforward<np_gm>
			, np_activityrank_operator
			, np_get_rank
			, mforward<pbnet::PROBUFF_NET_RANKLIST>
		>(e_ready_all);
	}

		bool actor_ranklist::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
		{
			return true;
		}
}// namespace ngl
