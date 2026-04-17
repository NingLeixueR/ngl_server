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
// File overview: Implements CSV client actor construction and table data caching.


#include "actor/actor_logic/actor_csvclient/actor_csvclient.h"
#include "actor/actor_logic/actor_csvserver/actor_csvserver.h"
#include "actor/actor_base/core/actor_manage.h"

namespace ngl
{
	actor_csvclient::actor_csvclient() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVCLIENT,
					.m_area = tab_self_area,
					.m_id	= nconfig.tid()
				},
				.m_weight = 0x7fffffff,
			})
	{}

	ENUM_ACTOR actor_csvclient::actor_type()
	{
		return ACTOR_CSVCLIENT;
	}

	i64_actorid actor_csvclient::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_csvclient::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 5 * 60) == false)
		{
			log_error()->print("actor_csvclient::init() make_timerparm::make_interval(tparm, 60) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	void actor_csvclient::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_csvclient::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_csvclient::nregister()
	{
		// Timer.
		actor::register_timer<actor_csvclient>(&actor_csvclient::timer_handle);
		
		// Bind custom np_ messages.
		register_handle<actor_csvclient
			, np_actor_reloadcsv
		>();
	}

	bool actor_csvclient::timer_handle([[maybe_unused]] const message<np_timerparm>&)
	{
		log_error()->print("############actor_csvclient::timer_handle###########");
		auto pro = std::make_shared<np_actor_csv_verify_version>();
		ncsv::foreach_verify(pro->m_verify);
		send_actor(actor_csvserver::actorid(), id_guid(), pro);
		return true;
	}
}//namespace ngl
