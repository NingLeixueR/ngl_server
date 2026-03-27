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
// File overview: Implements logic for actor friends.


#include "actor/actor_logic/actor_friends/actor_friends.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"

namespace ngl
{
	actor_friends::actor_friends() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_FRIENDS,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_friends::actor_type()
	{
		return ACTOR_FRIENDS;
	}

	i64_actorid actor_friends::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_friends::init()
	{
		// Bind the DB structure: DB.set(this);
		m_friends.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_friends::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_friends>::instance_readall(this, tools::pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase"));
	}

	void actor_friends::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_friends>::instance(id_guid()).exit();
	}

	void actor_friends::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_friends::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
		actor_events_logic::event_func(actorid(), eevents_logic_rolelogin, eevents_logic_roleoffline);
	}

	void actor_friends::nregister()
	{
		// Timer.
		actor::register_timer<actor_friends>(&actor_friends::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_friends
			, np_eevents_logic_rolelogin
			, np_eevents_logic_roleoffline
			, mforward<pbnet::PROBUFF_NET_FRIEND_ADD>
			, mforward<pbnet::PROBUFF_NET_FRIEND_ERASE>
			, mforward<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>
			, mforward<pbnet::PROBUFF_NET_FRIEND>
		>(e_ready_all);
	}

	bool actor_friends::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}// namespace ngl
