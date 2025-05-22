#include "activity_drawcompliance.h"
#include "actor_activity_manage.h"
#include "ttab_calendar.h"
#include "activity.h"

namespace ngl
{
	std::map<EActivity, activity*> activity::m_activityall;
	activity_drawcompliance g_activity_drawcompliance;

	activity::activity() :
		m_activityid(-1)
		, m_activity(nullptr)
	{

	}

	activity::activity(int32_t acalendarid, int64_t activityid, int64_t atime, activitydb& adb) :
		m_activityid(activityid)
		, m_activity(nullptr)
	{
		tab_activity* tab = allcsv::tab<tab_activity>((int32_t)activityid);
		if (tab == nullptr)
		{
			log_error()->print("tab_activity id[{}] not find!!!", activityid);
			return;
		}
		auto itor = adb.data().find(activityid);
		if (itor != adb.data().end())
		{
			m_activity = &itor->second;
		}
		else
		{
			pbdb::db_activity ldb;
			ldb.set_m_id(activityid);
			ldb.set_m_beg(ttab_calendar::data::beg(atime));
			ldb.set_m_end(ttab_calendar::data::end(atime));
			ldb.set_m_start(false);
			ldb.set_m_finish(false);
			ldb.set_m_calendarid(acalendarid);
			m_activity = adb.add(activityid, ldb);
		}
	}	

	void activity::rolelogin(i64_actorid aroleid)
	{
		const tab_activity* ltab = tab();
		int32_t lday = day();
		auto& ldb_activity = m_activity->getconst();
		auto itor = ldb_activity.m_stat().find(aroleid);
		if (itor == ldb_activity.m_stat().end())
		{
			(*m_activity->get().mutable_m_stat())[aroleid];
			itor = ldb_activity.m_stat().find(aroleid);
		}
		const pbdb::db_activity_mstat& lmstat = itor->second;

		// ¿ªÆôÈÎÎñ
		for (int32_t i = 0; i < ltab->m_taskday.size(); ++i)
		{
			auto& item = ltab->m_taskday[i];
			if (lday >= item.m_begday && lday < item.m_endday)
			{
				auto itoropen = lmstat.m_opentask().find(i);
				if (itoropen == lmstat.m_opentask().end())
				{
					auto pro = std::make_shared<mforward<np_operator_task>>(actor_activity_manage::actorid());
					np_operator_task* lnp = pro->add_data();
					lnp->m_isreceive = true;
					lnp->m_taskids = item.m_taskids;
					lnp->m_msg = std::format("{}:{}", activityid(), i);
					actor::static_send_actor(aroleid, nguid::make(), pro);
				}
			}
			if (lday >= item.m_endday)
			{
				auto itoropen = lmstat.m_opentask().find(i);
				auto itorclose = lmstat.m_closetask().find(i);
				if (itoropen != lmstat.m_opentask().end() && itorclose == lmstat.m_closetask().end())
				{
					auto pro = std::make_shared<mforward<np_operator_task>>(actor_activity_manage::actorid());
					np_operator_task* lnp = pro->add_data();
					lnp->m_isreceive = false;
					lnp->m_taskids = item.m_taskids;
					lnp->m_msg = std::format("{}:{}", activityid(), i);
					actor::static_send_actor(aroleid, nguid::make(), pro);
				}
			}
		}
	}

}//namespace ngl