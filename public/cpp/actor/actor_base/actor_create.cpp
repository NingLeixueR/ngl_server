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
#include "actor_create.h"

namespace ngl
{
	actor_create::actor_create() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CREATE,
					.m_area = tab_self_area,
					.m_id = nconfig::m_tcount,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	i64_actorid actor_create::actorid(i32_serverid aserverid)
	{
		return nguid::make(ACTOR_CREATE, tab_self_area, aserverid);
	}

	void actor_create::nregister()
	{
		register_handle<actor_create>::func<
			np_actorswitch_process<np_actorswitch_process_role>
		>(false);
	}
}//namespace ngl