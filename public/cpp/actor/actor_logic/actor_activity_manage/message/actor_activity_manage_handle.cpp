#include "actor_activity_manage.h"
#include "actor_keyvalue.h"
#include "ttab_calendar.h"
#include "actor_brief.h"
#include "activity.h"
#include "net.pb.h"
namespace ngl
{
	bool actor_activity_manage::handle(const message<np_actor_activity>& adata)
	{
		const np_actor_activity& lrecv = *adata.get_data();
		for (i64_actorid item : lrecv.m_activityids)
		{
			if (lrecv.m_start)
			{
				activity_start(item, lrecv.m_time, lrecv.m_calendarid);
			}
			else
			{
				activity_finish(item, lrecv.m_time, lrecv.m_calendarid);
			}
		}
		return true;
	}
}//namespace ngl
