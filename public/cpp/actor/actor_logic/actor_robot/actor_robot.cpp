/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_robot_manage.h"
#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

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