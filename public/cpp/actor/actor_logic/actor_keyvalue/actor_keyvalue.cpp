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
// File overview: Implements key-value store actor construction and storage operations.


#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/generated/pb/net.pb.h"

namespace ngl
{
	actor_keyvalue::actor_keyvalue() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_KEYVALUE,
						.m_area = tab_self_area,
						.m_manage_dbclient = true
					},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_keyvalue::actor_type()
	{
		return ACTOR_KEYVALUE;
	}

	i64_actorid actor_keyvalue::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_keyvalue::init()
	{
		// Bind the DB structure: DB.set(this);
		m_keyvaluedb.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_keyvalue::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		tdb_keyvalue::nsp_ser::init(&m_keyvaluedb);
	}

	void actor_keyvalue::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_keyvalue::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_keyvalue::nregister()
	{
		// Timer.
		actor::register_timer<actor_keyvalue>(&actor_keyvalue::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_keyvalue
			, mforward<np_gm>
		>();
	}

	bool actor_keyvalue::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl
