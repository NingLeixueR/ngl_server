#include "ttab_calendar.h"
#include "actor_calendar.h"
#include "calendar_function.h"
#include "xmlinfo.h"

namespace ngl
{
	std::map<int32_t, ttab_calendar::data> ttab_calendar::m_data;

	void send_calendar(tab_calendar* atab, int64_t autc, bool astart)
	{
		std::shared_ptr<np_calendar> pro(new np_calendar());
		pro->m_calendarid = atab->m_id;
		pro->m_start = astart;
		pro->m_time = autc;
		actor::static_send_actor(nguid::make(ACTOR_CALENDAR, ttab_servers::tab()->m_area, nguid::none_actordataid()), nguid::make(), pro);
	}

	void ttab_calendar_post(tab_calendar* atab, int64_t autc, bool astart)
	{
		int lnow = localtime::gettime();
		
		int ltime = lnow - (astart? ttab_calendar::data::beg(autc) : ttab_calendar::data::end(autc));
		if (ltime < 0)
		{
			send_calendar(atab, autc, astart);
			return;
		}
		wheel_parm lparm
		{
			.m_ms = ltime * 1000,
			.m_intervalms = [ltime](int64_t) {return ltime * 1000; } ,
			.m_count = 1,
			.m_fun = [atab, autc, astart](wheel_node* anode)
			{
				send_calendar(atab, autc, astart);
			}
		};
		calendar_wheel.addtimer(lparm);

	}

	void ttab_calendar::post(tab_calendar* atab, int64_t autc, pbdb::db_calendar& acalendar)
	{
		if (acalendar.m_start() == false)
		{
			ttab_calendar_post(atab, autc, true);
		}
		ttab_calendar_post(atab, autc, false);
	}
}// namespace ngl