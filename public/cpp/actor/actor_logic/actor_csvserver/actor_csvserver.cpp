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
// File overview: Implements logic for actor csvserver.


#include "actor/actor_logic/actor_csvserver/actor_csvserver.h"

namespace ngl
{
	actor_csvserver::actor_csvserver() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVSERVER,
				},
				.m_weight = 0x7fffffff,
			})
	{}

	ENUM_ACTOR actor_csvserver::actor_type()
	{
		return ACTOR_CSVSERVER;
	}

	i64_actorid actor_csvserver::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_csvserver::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_csvserver::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_csvserver::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_csvserver::nregister()
	{
		// Timer.
		actor::register_timer<actor_csvserver>(&actor_csvserver::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_csvserver
			, np_actor_csv_verify_version
		>(e_ready_all);
	}

	bool actor_csvserver::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl