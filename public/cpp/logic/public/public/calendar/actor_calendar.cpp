#include "calendar_function.h"
#include "actor_calendar.h"
#include "ttab_calendar.h"

namespace ngl
{
	actor_calendar::actor_calendar() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CALENDAR,
					.m_area = ttab_servers::tab()->m_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	void actor_calendar::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_calendar>::func<
			np_calendar
		>(true);
	}

	bool actor_calendar::handle(const message<np_calendar>& adata)
	{
		const np_calendar& recv = *adata.get_data();
		tab_calendar* tab = ttab_calendar::tab(recv.m_calendarid);
		if (tab == nullptr)
		{
			return true;
		}
		pbdb::db_calendar* lcalendar = m_calendar.get_calendar(recv.m_calendarid);
		if (lcalendar == nullptr)
		{
			return true;
		}
		if (recv.m_start)
		{
			calendar_function::start(tab, recv.m_time, lcalendar);
		}
		else
		{
			calendar_function::finish(tab, recv.m_time, lcalendar);
			m_calendar.next_calendar(recv.m_calendarid, recv.m_time);
		}
		return true;
	}
}// namespace ngl