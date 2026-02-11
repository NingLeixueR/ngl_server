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
#include "actor_manage.h"
#include "ntimer.h"

namespace ngl
{
	int ntimer::addtimer(actor_base* actor, const std::shared_ptr<np_timerparm>& aparm)
	{
		i64_actorid lidguid = actor->id_guid();
		wheel_parm lparm
		{
			.m_ms			= (int32_t)(aparm->m_ms),
			.m_intervalms	= aparm->m_intervalms,
			.m_count		= aparm->m_count,
			.m_fun			= [lidguid, aparm](const wheel_node* anode)
			{
				if (!actor_manage::instance().is_have_actor(lidguid))
				{
					log_error()->print("ntimer run fail: {}#{}", (nguid)lidguid, aparm->m_timerid);
					twheel::wheel().removetimer(aparm->m_timerid);
					return;
				}
				aparm->m_triggerms = anode->m_parm.m_timerstart + anode->m_parm.m_ms;
				actor::send_actor<np_timerparm, false>(lidguid, nguid::make(), aparm);
			}
		};
		aparm->m_timerid = (int)twheel::wheel().addtimer(lparm);
		return aparm->m_timerid;
	}
}//namespace ngl