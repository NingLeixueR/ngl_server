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
// File overview: Implements logic for actor family.


#include "actor/actor_logic/actor_family/actor_family.h"

namespace ngl
{
	actor_family::actor_family() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_FAMILY,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_family::actor_type()
	{
		return ACTOR_FAMILY;
	}

	i64_actorid actor_family::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_family::init()
	{
		// Bind the DB structure: DB.set(this);
		m_familyer.set(this);
		m_family.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_family::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_family>::instance_readall(this, pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase"));
		m_drop.init(this, {});

	}

	void actor_family::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_family>::instance(id_guid()).exit();
		m_drop.exit();
	}

	void actor_family::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_family::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
		actor_events_logic::event_func(actorid(), eevents_logic_rolelogin, eevents_logic_roleoffline);
	}

	void actor_family::nregister()
	{
		// Timer.
		actor::register_timer<actor_family>(&actor_family::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_family
			, mforward<np_gm>
			, np_eevents_logic_rolelogin
			, np_eevents_logic_roleoffline
			, mforward<pbnet::PROBUFF_NET_FAMIL_CREATE>
			, mforward<pbnet::PROBUFF_NET_FAMIL_JOIN>
			, mforward<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>
			, mforward<pbnet::PROBUFF_NET_FAMIL_CEDE>
			, mforward<pbnet::PROBUFF_NET_FAMIL_LEAVE>
			, mforward<pbnet::PROBUFF_NET_FAMIL_LIST>
			, mforward<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>
			, mforward<pbnet::PROBUFF_NET_FAMIL_SIGN>
			, mforward<pbnet::PROBUFF_NET_FAMIL_INFO>
		>(e_ready_all);
	}

	bool actor_family::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}// namespace ngl
