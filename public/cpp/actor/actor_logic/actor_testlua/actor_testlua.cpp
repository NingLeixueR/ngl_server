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
// File overview: Implements logic for actor testlua.


#include "actor/actor_logic/actor_testlua/actor_testlua.h"

namespace ngl
{
	actor_testlua::actor_testlua() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_TESTLUA,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
					.m_enscript = enscript_lua,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_testlua::actor_type()
	{
		return ACTOR_TESTLUA;
	}

	i64_actorid actor_testlua::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_testlua::init()
	{
		// Bind the DB structure: DB.set(this);
		m_testlua.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_testlua::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		// # Csvdatawritelua
		//nscript_data_csv<tab_servers> ltab(ttab_servers::instance().tablecsv());
		//nscript_data_push("csv", ltab);

		
		//tdb_brief::nsp_cread<actor_testlua>::instance_readpart(this, { ngl::tools::nguidstr2int64("actor_brief#1#1") });
	}

	void actor_testlua::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_testlua>::instance(id_guid()).exit();
	}
	
	void actor_testlua::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_testlua::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_testlua::nregister()
	{
		// Timer.
		actor::register_timer<actor_testlua>(&actor_testlua::timer_handle);

		register_script_handle<actor_testlua
			, np_testlua
		>(e_ready_all);
		register_script_handle<actor_testlua
			, mforward<pbnet::PROBUFF_NET_TESTLUA>
		>(e_ready_all);
	}

	bool actor_testlua::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}// namespace ngl
