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
// File overview: Implements logic for actor robot.


#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "actor/actor_logic/actor_robot/actor_robot.h"
#include "actor/actor_base/nregister.h"
#include "actor/actor_logic/nforward.h"

namespace ngl
{
	actor_robot::actor_robot(i16_area aarea, i32_actordataid arobotid, void*) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROBOT,
					.m_area = aarea,
					.m_id = arobotid,
				},
				.m_weight = 0x7fffffff,
			})
	{
		if (aarea != tab_self_area)
		{
			tools::no_core_dump();
			return;
		}
	}

	ENUM_ACTOR actor_robot::actor_type()
	{
		return ACTOR_ROBOT;
	}

	i64_actorid actor_robot::actorid(int32_t adataid)
	{
		return nguid::make(actor_type(), tab_self_area, adataid);
	}

	void actor_robot::init()
	{
	}

	void actor_robot::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_robot::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_robot::nregister()
	{
		actor::register_timer<actor_robot>(&actor_robot::timer_handle);

		register_handle<actor_robot
			, np_thruput_test
			, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE
		>(e_ready_all);

		nforward::g2c();
	}

	void actor_robot::ukcp_connect(pbnet::ENUM_KCP akcpenum)
	{
		int16_t lservertid = 0;
		int16_t ltcount = 0;
		i64_actorid lactorid = nguid::make();
		switch (akcpenum)
		{
		case pbnet::ENUM_KCP::KCP_GATEWAY:
		{
			lservertid = nnodeid::tid(m_robot->m_gatewayid);
			ltcount = nnodeid::tcount(m_robot->m_gatewayid);
		}
		break;
		case pbnet::ENUM_KCP::KCP_ROLE:
		{
			lservertid = nnodeid::tid(m_robot->m_gameid);
			ltcount = nnodeid::tcount(m_robot->m_gameid);
			lactorid = nguid::make_type(id_guid(), ACTOR_ROLE);
		}
		break;
		}
		std::string lcmd = std::format("kcp {} {} {} {}", (int32_t)akcpenum, lservertid, ltcount, lactorid);
		std::vector<std::string> lvec;
		if (!ngl::tools::splite(lcmd.c_str(), " ", lvec))
		{
			return;
		}
		ngl::actor_robot_manage::parse_command(lvec);
	}

	bool actor_robot::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl