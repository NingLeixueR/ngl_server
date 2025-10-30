/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ÏîÄ¿Ãû³Æ£ºngl_server
* ÏîÄ¿µØÖ·£ºhttps://github.com/NingLeixueR/ngl_server
* 
* ±¾ÎÄ¼şÊÇ ngl_server ÏîÄ¿µÄÒ»²¿·Ö£¬×ñÑ­ MIT ¿ªÔ´Ğ­Òé·¢²¼¡£
* Äú¿ÉÒÔ°´ÕÕĞ­Òé¹æ¶¨×ÔÓÉÊ¹ÓÃ¡¢ĞŞ¸ÄºÍ·Ö·¢±¾ÏîÄ¿£¬°üÀ¨ÉÌÒµÓÃÍ¾£¬
* µ«Ğè±£ÁôÔ­Ê¼°æÈ¨ºÍĞí¿ÉÉùÃ÷¡£
* 
* Ğí¿ÉÏêÇé²Î¼ûÏîÄ¿¸ùÄ¿Â¼ÏÂµÄ LICENSE ÎÄ¼ş£º
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
ï»¿#include "actor_manage.h"
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
					log_error()->print("time run fail: {} , {}", (nguid)lidguid, aparm->m_timerid);
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
