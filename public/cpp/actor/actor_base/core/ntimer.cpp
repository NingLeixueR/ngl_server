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
// File overview: Implements timer scheduling, tick processing, and callback dispatch.

#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ntimer.h"

namespace ngl
{
	int64_t ntimer::addtimer(actor_base* actor, const std::shared_ptr<np_timerparm>& aparm)
	{
		i64_actorid lidguid = actor->id_guid();
		tools::wheel_parm lparm
		{
			.m_ms			= aparm->m_ms,
			.m_intervalms	= aparm->m_intervalms,
			.m_count		= aparm->m_count,
			.m_fun			= [lidguid, aparm](const tools::wheel_node* anode)
			{
				if (!actor_manage::instance().is_have_actor(lidguid))
				{
					log_error()->print("ntimer run fail: {}#{}", (nguid)lidguid, aparm->m_timerid);
					tools::twheel::wheel().removetimer(aparm->m_timerid);
					return;
				}
				aparm->m_triggerms = anode->m_parm.m_timerstart + anode->m_parm.m_ms;
				actor::send_actor<np_timerparm, false>(lidguid, nguid::make(), aparm);
			}
		};
		aparm->m_timerid = tools::twheel::wheel().addtimer(lparm);
		return aparm->m_timerid;
	}
}//namespace ngl
