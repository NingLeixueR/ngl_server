#include "actor_activity_manage.h"
#include "calendar_function.h"
#include "nprotocol.h"
#include "actor.h"

namespace ngl
{
	std::array<calendar_function::tcall, ECalendarType::ECalendarTypeCount> calendar_function::m_call;
	time_wheel calendar_wheel;

	bool calendar_function::operator_activ(tab_calendar* tab, pbdb::db_calendar& acalendar, int64_t atime, bool astart, i64_actorid aactor)
	{
		auto pro = std::make_shared<np_calendar_actor_activity>();
		tools::splite(tab->m_carendarparm.c_str(), "*", pro->m_activityids);
		pro->m_info.m_calendarid = tab->m_id;
		pro->m_info.m_start = astart;
		pro->m_info.m_time = atime;
		actor::static_send_actor(actor_activity_manage::actorid(), nguid::make(), pro);
		return true;
	}

	bool calendar_function::operator_task(tab_calendar* tab, pbdb::db_calendar& acalendar, int64_t atime, bool astart, i64_actorid aactor)
	{
		auto pro = std::make_shared<np_calendar_actor_task>();
		tools::splite(tab->m_carendarparm.c_str(), "*", pro->m_taskids);
		pro->m_info.m_calendarid = tab->m_id;
		pro->m_info.m_start = astart;
		pro->m_info.m_time = atime;
		if (aactor == nguid::make())
		{
			actor::static_send_actor(ACTOR_ROLE, pro, true);
		}
		else
		{
			actor::static_send_actor(aactor, nguid::make(), pro);
		}
		return true;
	}

	void calendar_function::push(
		ECalendarType atype,
		const std::function<bool(tab_calendar*, pbdb::db_calendar&, int32_t, i64_actorid)>& astart,
		const std::function<bool(tab_calendar*, pbdb::db_calendar&, int32_t, i64_actorid)>& afinish
	)
	{
		m_call[atype] = std::make_tuple(astart, afinish);
	}

	void calendar_function::init()
	{
		push(ECalendarTypeActivity,
			[](tab_calendar* tab, pbdb::db_calendar& acalendar, int32_t atime, i64_actorid aactor)->bool
			{
				return operator_activ(tab, acalendar, atime, true, aactor);
			},
			[](tab_calendar* tab, pbdb::db_calendar& acalendar, int64_t atime, i64_actorid aactor)->bool
			{
				return operator_activ(tab, acalendar, atime, true, aactor);
			});
		push(ECalendarTypeTask,
			[](tab_calendar* tab, pbdb::db_calendar& acalendar, int64_t atime, i64_actorid aactor)->bool
			{
				return operator_activ(tab, acalendar, atime, true, aactor);
			},
			[](tab_calendar* tab, pbdb::db_calendar& acalendar, int64_t atime, i64_actorid aactor)->bool
			{
				return operator_activ(tab, acalendar, atime, true, aactor);
			});
	}
}//namespace ngl