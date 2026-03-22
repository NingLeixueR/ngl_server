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
// File overview: Implements logic for actor gm.


#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_gm/actor_gm.h"

namespace ngl
{
	actor_gm::actor_gm() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GM,
					.m_area = tab_self_area,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_gm::actor_type()
	{
		return ACTOR_GM;
	}

	i64_actorid actor_gm::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, nguid::none_actordataid());
	}

	void actor_gm::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gm::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}
	
	void actor_gm::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_gm::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_gm::nregister()
	{
		// Timer.
		actor::register_timer<actor_gm>(&actor_gm::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_gm
			, ngl::np_gm
			, mforward<ngl::np_gm_response>
		>(e_ready_all);
	}

	void actor_gm::sendbytype(ENUM_ACTOR atype, const pack* apack, const ngl::np_gm& apro)
	{
		send_actor(atype, std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro));
	}

	void actor_gm::sendbyactorid(i64_actorid aactorid, const pack* apack, const ngl::np_gm& apro)
	{
		send_actor(aactorid, id_guid(), std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro));
	}

	bool actor_gm::checklocalbytype(NODE_TYPE atype)
	{
		return ttab_servers::instance().const_tab()->m_type == atype;
	}

	bool actor_gm::sendtophp(i32_sessionid aidentifier, const ngl::np_gm_response& adata)
	{
		log_error()->print("gm2php [{}]", adata.m_json);
		actor::send(aidentifier, adata, nguid::make(), nguid::make());
		return true;
	}

	bool actor_gm::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}// namespace ngl
