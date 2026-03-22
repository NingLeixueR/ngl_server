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
// File overview: Implements logic for actor gmclient.


#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"

namespace ngl
{
	actor_gmclient::actor_gmclient() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GMCLIENT,
					.m_area = tab_self_area,
					.m_id = nconfig.nodeid(),
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_gmclient::actor_type()
	{
		return ACTOR_GMCLIENT;
	}

	i64_actorid actor_gmclient::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, nconfig.nodeid());
	}

	void actor_gmclient::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gmclient::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_gmclient::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_gmclient::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_gmclient::allprotocol(protocols& apro)
	{
		tprotocol::allprotocol(apro.m_data);
	}

	void actor_gmclient::nregister()
	{
		// Timer.
		actor::register_timer<actor_gmclient>(&actor_gmclient::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_gmclient
			, mforward<np_gm>
		>(e_ready_all);
	}

	bool actor_gmclient::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl
