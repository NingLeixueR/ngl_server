#include "ttab_calendar.h"
#include "actor_calendar.h"
#include "calendar_function.h"
#include "xmlinfo.h"

namespace ngl
{
	std::map<int32_t, ttab_calendar::data> ttab_calendar::m_data;

	void operator_calendar(tab_calendar* atab, int32_t autc, bool astart)
	{
		actor_calendar::getInstance().operatpr_calendar(atab->m_id, autc, astart);
	}

	void ttab_calendar_post(tab_calendar* atab, int64_t autc, bool astart)
	{
		int lnow = (int)localtime::gettime();
		
		int ltime = (astart ? ttab_calendar::data::beg(autc) : ttab_calendar::data::end(autc)) - lnow;
		if (ltime < 0)
		{
			operator_calendar(atab, autc, astart);
			return;
		}
		wheel_parm lparm
		{
			.m_ms = ltime * 1000,
			.m_intervalms = [ltime](int64_t) {return ltime * 1000; } ,
			.m_count = 1,
			.m_fun = [atab, autc, astart](const wheel_node* anode)
			{
				operator_calendar(atab, autc, astart);
			}
		};
		calendar_wheel.addtimer(lparm);
	}

	void ttab_calendar::post(tab_calendar* atab, int32_t autc, bool astart)
	{
		ttab_calendar_post(atab, autc, astart);
	}
}// namespace ngl