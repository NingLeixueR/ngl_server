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
// File overview: Implements extended Lua test actor for advanced binding scenarios.


#include "actor/actor_logic/actor_testlua2/actor_testlua2.h"

namespace ngl
{
	actor_testlua2::actor_testlua2() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_TESTLUA2,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
					.m_enscript = enscript_lua,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_testlua2::actor_type()
	{
		return ACTOR_TESTLUA2;
	}

	i64_actorid actor_testlua2::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_testlua2::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_testlua::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		// Export CSV table data to Lua (commented out).
		//nscript_data_csv<tab_servers> ltab(ttab_servers::instance().tablecsv());
		//nscript_data_push("csv", ltab);
	}

	void actor_testlua2::erase_actor_before()
	{
	}
	
	void actor_testlua2::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_testlua2::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_testlua2::nregister()
	{
		// Timer.
		actor::register_timer<actor_testlua2>(&actor_testlua2::timer_handle);

		register_script_handle<actor_testlua2
			, np_testlua
		>();
	}

	bool actor_testlua2::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}// namespace ngl
