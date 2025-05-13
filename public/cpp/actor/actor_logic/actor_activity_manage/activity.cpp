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
}//namespace ngl