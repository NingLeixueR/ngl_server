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
#include "actor_gateway_c2g.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_gateway_c2g::actor_gateway_c2g() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_C2G,
					.m_area = tab_self_area,
					.m_id = nconfig.tcount(),
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	ENUM_ACTOR actor_gateway_c2g::actor_type()
	{
		return ACTOR_GATEWAY_C2G;
	}

	i64_actorid actor_gateway_c2g::actorid(i32_actordataid aactordataid)
	{
		return nguid::make(ACTOR_GATEWAY_C2G, tab_self_area, aactordataid);
	}

	void actor_gateway_c2g::init()
	{
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gateway_c2g::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_gateway_c2g::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_gateway_c2g::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_gateway_c2g::nregister()
	{
		actor::register_timer<actor_gateway_c2g>(&actor_gateway_c2g::timer_handle);

		register_handle<actor_gateway_c2g
			, np_actor_gatewayinfo_updata
		>(e_ready_all);

		nforward::c2g();
	}

	bool actor_gateway_c2g::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl