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
// File overview: Implements logic for actor role manage.


#include "actor/actor_logic/actor_role_manage/actor_role_manage.h"
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_base/core/nregister.h"
#include "actor/actor_base/core/ntimer.h"

namespace ngl
{
	actor_role_manage::actor_role_manage() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROLE_MANAGE,
					.m_area = tab_self_area,
					.m_id = nconfig.tcount()
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_role_manage::actor_type()
	{
		return ACTOR_ROLE_MANAGE;
	}

	i64_actorid actor_role_manage::actorid()
	{
		return nguid::make(actor_type(), nguid::none_area(), nconfig.tcount());
	}

	void actor_role_manage::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_role_manage::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		std::set<i32_fieldnumber> lfieldset;
		tdb_keyvalue::nsp_cwrite<actor_role_manage>::instance_writepart(
			this
			, { }
			, tools::pb_field::field_number<pbdb::db_keyvalue>(lfieldset, "mvalue")
			, { }
			, { pbdb::db_keyvalue_ekv_account_ban }		
		);
		
		tdb_keyvalue::nsp_cwrite<actor_role_manage>::instance(id_guid()).set_changedatafun( 
			[this](int64_t aid, const pbdb::db_keyvalue& akeyval, bool afirstsynchronize)
			{
				log_error()->print(
					"actor_manage_role nclient_keyvalue::set_changedata_fun####### [{}:{}:{}]"
					, aid, akeyval.mvalue().c_str()
					, afirstsynchronize ? "first" : "change"
				);
				if (afirstsynchronize)
				{
					tools::splite(akeyval.mvalue().c_str(), "*", m_roleban);
				}
			});
	}

	void actor_role_manage::handle_after(handle_pram&)
	{
		tdb_brief::nsp_cwrite<actor_role_manage>::change(id_guid());
	}

	void actor_role_manage::erase_actor_before()
	{
		tdb_keyvalue::nsp_cread<actor_role_manage>::instance(id_guid()).exit();
	}

	void actor_role_manage::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_role_manage::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_role_manage::nregister()
	{
		// Timer.
		actor::register_timer<actor_role_manage>(&actor_role_manage::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_role_manage
			, mforward<np_gm>
			, pbnet::PROBUFF_NET_ROLE_LOGIN
		>(e_ready_all);
	}

	bool actor_role_manage::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl
