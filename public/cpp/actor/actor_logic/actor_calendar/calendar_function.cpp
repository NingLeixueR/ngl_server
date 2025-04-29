#include "actor_activity_manage.h"
#include "calendar_function.h"
#include "nprotocol.h"
#include "actor.h"

namespace ngl
{
	std::map<ECalendarType, calendar_function::callback> calendar_function::m_fun;
	time_wheel calendar_wheel;

	void calendar_function::operator_activ(tab_calendar* tab, int64_t atime, bool astart)
	{
		auto pro = std::make_shared<np_actor_activity>();
		tools::splite(tab->m_carendarparm.c_str(), "*", pro->m_activityids);
		pro->m_calendarid = tab->m_id;
		pro->m_start = astart;
		pro->m_time = atime;
		actor::static_send_actor(actor_activity_manage::actorid(), nguid::make(), pro);
	}

	void calendar_function::operator_task(tab_calendar* tab, int64_t atime, bool astart)
	{
		auto pro = std::make_shared<np_actor_task>();
		tools::splite(tab->m_carendarparm.c_str(), "*", pro->m_taskids);
		pro->m_calendarid = tab->m_id;
		pro->m_start = astart;
		pro->m_time = atime;
		
		actor::static_send_actor(ACTOR_ROLE, pro, true);
	}

	void calendar_function::init()
	{
		m_fun[ECalendarTypeActivity] = {
			.m_start = [](tab_calendar* tab, int64_t atime)
			{
				operator_activ(tab, atime, true);
			},
			.m_finish = [](tab_calendar* tab, int64_t atime)
			{
				operator_activ(tab, atime, false);
			}
		};

		m_fun[ECalendarTypeTask] = {
			.m_start = [](tab_calendar* tab, int64_t atime)
			{
				operator_activ(tab, atime, true);
			},
			.m_finish = [](tab_calendar* tab, int64_t atime)
			{
				operator_activ(tab, atime, false);
			}
		};
	}
}//namespace ngl