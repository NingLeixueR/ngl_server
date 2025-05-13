#include "actor_activity_manage.h"
#include "actor_calendar.h"
#include "actor_keyvalue.h"
#include "ttab_calendar.h"
#include "actor_brief.h"
#include "activity.h"
#include "net.pb.h"
namespace ngl
{
	bool actor_activity_manage::handle(const message<np_calendar_actor_activity>& adata)
	{
		const np_calendar_actor_activity& lrecv = *adata.get_data();
		for (i64_actorid item : lrecv.m_activityids)
		{
			if (lrecv.m_info.m_start)
			{
				activity_start(item, lrecv.m_info.m_time, lrecv.m_info.m_calendarid);
			}
			else
			{
				activity_finish(item, lrecv.m_info.m_time, lrecv.m_info.m_calendarid);
			}
		}
		auto pro = std::make_shared<mforward<np_calendar_actor_respond>>(id_guid());
		np_calendar_actor_respond* lpdata = pro->add_data();
		lpdata->m_info = lrecv.m_info;
		send_actor(actor_calendar::actorid(), pro);
		return true;
	}
}//namespace ngl
