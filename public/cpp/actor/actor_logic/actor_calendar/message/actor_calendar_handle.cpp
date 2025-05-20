#include "calendar_function.h"
#include "actor_calendar.h"
#include "ttab_calendar.h"
namespace ngl
{
	bool actor_calendar::operatpr_calendar(int32_t acalendarid, int32_t atime, bool astart)
	{
		const tab_calendar* tab = ttab_calendar::tab(acalendarid);
		if (tab == nullptr)
		{
			return true;
		}
		data_modified<pbdb::db_calendar>* lcalendar = m_calendar.get_calendar(acalendarid);
		if (lcalendar == nullptr)
		{
			return true;
		}
		calendar_function::trigger(tab, lcalendar->get(), atime, astart);
		if (!astart)
		{
			m_calendar.next_calendar(acalendarid, atime);
		}
		m_calendar.add_trigger_list(acalendarid, atime, astart);
		return true;
	}
	bool actor_calendar::handle(const message<mforward<np_actor_calendar_requst>>& adata)
	{
		i64_actorid lidentifier = adata.get_data()->identifier();;
		const np_actor_calendar_requst& recv = *adata.get_data()->data();
		std::map<int64_t, std::vector<pbdb::calendar_pair>> ltriggerlist;
		m_calendar.get_trigger_list(lidentifier, ltriggerlist);

		for (const auto& item1 : ltriggerlist)
		{
			const tab_calendar* tab = ttab_calendar::tab(item1.first);
			if (tab == nullptr)
			{
				continue;
			}
			for (const auto& item2 : item1.second)
			{
				data_modified<pbdb::db_calendar>* lpcalendar = m_calendar.get_calendar(item1.first);
				if (lpcalendar == nullptr)
				{
					continue;
				}
				calendar_function::trigger(tab, lpcalendar->get(), item2.m_triggertime(), item2.m_isstart(), lidentifier);
			}
		}

		return true;
	}
	bool actor_calendar::handle(const message<mforward<np_calendar_actor_respond>>& adata)
	{
		i64_actorid lidentifier = adata.get_data()->identifier();;
		const np_calendar_actor_respond& recv = *adata.get_data()->data();
		m_calendar.add_actor_trigger(lidentifier, recv.m_info);
		return true;
	}
}//namespace ngl
