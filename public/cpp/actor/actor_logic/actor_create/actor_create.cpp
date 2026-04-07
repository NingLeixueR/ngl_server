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
// File overview: Implements logic for actor create.

#include "actor/actor_logic/actor_create/actor_create.h"

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
					.m_id = nconfig.tcount(),
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
		register_handle<actor_create
			, np_actorswitch_process<np_actorswitch_process_role>
		>();
	}
}//namespace ngl
