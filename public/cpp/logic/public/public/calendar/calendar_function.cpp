#include "calendar_function.h"
#include "nprotocol.h"
#include "actor.h"

namespace ngl
{
	std::map<ECalendarType, calendar_function::callback> calendar_function::m_fun;
	time_wheel calendar_wheel;

	void calendar_function::init()
	{
		m_fun[ECalendarTypeEveryDay] = {
			.m_start = [](tab_calendar* tab, int64_t atime)
			{

			},
			.m_finish = [](tab_calendar* tab, int64_t atime)
			{

			}
		};

		auto lfunactivity = [](tab_calendar* tab, int64_t atime, bool astart)
		{
				auto pro = std::make_shared<np_actor_activity>();
				pro->m_activityids.insert(pro->m_activityids.begin(), tab->m_carendarparm.begin(), tab->m_carendarparm.end());
				pro->m_calendarid = tab->m_id;
				pro->m_start = astart;
				pro->m_time = atime;
				actor::static_send_actor(nguid::make(ACTOR_ACTIVITY_MANAGE, ttab_servers::tab()->m_area, nguid::none_actordataid())
					, nguid::make()
					, pro
				);
		};

		//np_actor_openactivity
		m_fun[ECalendarTypeActivity] = {
			.m_start = [lfunactivity](tab_calendar* tab, int64_t atime)
			{
				lfunactivity(tab, atime, true);
			},
			.m_finish = [lfunactivity](tab_calendar* tab, int64_t atime)
			{
				lfunactivity(tab, atime, false);
			}
		};
	}
}