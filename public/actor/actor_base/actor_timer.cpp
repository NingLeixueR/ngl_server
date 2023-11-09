#include "actor_timer.h"
#include "actor_manage.h"


namespace ngl
{
	bool actor_timer::check_actor_valid(i64_actorid lidguid)
	{
		return actor_manage::getInstance().is_have_actor(lidguid);
	}

	int actor_timer::addtimer(actor_base* actor, std::shared_ptr<timerparm>& aparm)
	{
		i64_actorid lidguid = actor->id_guid();
		wheel_parm lparm
		{
			.m_ms = (int32_t)(aparm->m_ms),
			.m_intervalms = aparm->m_intervalms,
			.m_count = aparm->m_count,
			.m_fun = [lidguid, aparm](wheel_node* anode)
			{
				if (check_actor_valid(lidguid) == false)
				{
					LogLocalError("time run fail: actorid[%]", lidguid);
					return;
				}
				
				aparm->m_triggerms = anode->m_parm.m_timerstart + anode->m_parm.m_ms;
				std::shared_ptr<timerparm> ltemp = aparm;
				handle_pram lpram;
				handle_pram::create<timerparm, false>(lpram, lidguid, actor_guid::moreactor(), ltemp);
				actor_manage::getInstance().push_task_id(lidguid, lpram, false);
			}
		};
		aparm->m_timerid = twheel::wheel().addtimer(lparm);
		return aparm->m_timerid;
	}
}
