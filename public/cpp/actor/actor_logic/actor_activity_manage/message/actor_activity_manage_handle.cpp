#include "actor_activity_manage.h"
#include "actor_calendar.h"
#include "actor_keyvalue.h"
#include "ttab_calendar.h"
#include "actor_brief.h"
#include "activity.h"
#include "net.pb.h"
namespace ngl
{
	bool actor_activity_manage::handle(const message<mforward<np_operator_task_response>>& adata)
	{
		i64_actorid roleid = adata.get_data()->identifier();
		const np_operator_task_response* lrecv = adata.get_data()->data();
		int32_t activityid = 0;
		int32_t lindex = 0;
		if (!tools::splite(lrecv->m_msg.c_str(), ":", activityid, lindex))
		{
			return true;
		}
		auto& lactivity = get_activity(activityid);
		if (lactivity == nullptr)
		{
			return true;
		}
		lactivity->recv_task_response(roleid, lindex, lrecv->m_isreceive);
		return true;
	}
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
	bool actor_activity_manage::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		const np_eevents_logic_rolelogin* lrecv = adata.get_data();
		for (auto itor = m_activitys.begin(); itor != m_activitys.end(); ++itor)
		{
			itor->second->rolelogin(lrecv->m_actorid);
		}

		return true;
	}
}//namespace ngl
