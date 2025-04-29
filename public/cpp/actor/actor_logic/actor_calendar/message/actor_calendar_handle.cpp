#include "calendar_function.h"
#include "actor_calendar.h"
#include "ttab_calendar.h"
namespace ngl
{
	bool actor_calendar::handle(const message<mforward<np_actor_calendar_requst>>& adata)
	{
		i64_actorid roleid = adata.get_data()->identifier();;
		const np_actor_calendar_requst& recv = *adata.get_data()->data();
		std::map<int64_t, std::vector<calendar_trigger>> ltriggerlist;
		m_calendar.get_trigger_list(recv.m_loginoututc, ltriggerlist);

		for (const auto& item1 : ltriggerlist)
		{
			tab_calendar* tab = ttab_calendar::tab(item1.first);
			if (tab == nullptr)
			{
				continue;
			}
			for (const auto& item2 : item1.second)
			{
				calendar_function::trigger(tab, item2.m_triggerutc, item2.m_isstart);
			}
		}

		return true;
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
		calendar_function::trigger(tab, recv.m_time, recv.m_start, lcalendar);
		if (recv.m_start == false)
		{
			m_calendar.next_calendar(recv.m_calendarid, recv.m_time);
		}
		m_calendar.add_trigger_list(recv.m_calendarid, recv.m_time, recv.m_start);
		return true;
	}
}//namespace ngl
