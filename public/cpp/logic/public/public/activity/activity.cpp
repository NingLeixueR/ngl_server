#include "activity_drawcompliance.h"
#include "actor_manage_activity.h"
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

	void activity::post(bool astart)
	{
		if ((astart ? is_start() : is_finish()) == false && calendarid() < 0)
		{
			int32_t lnow = (int32_t)localtime::gettime();
			int32_t ltime = astart ? start_utc() : finish_utc();
			int32_t ltemp = ltime - lnow;
			if (ltemp < 0)
			{
				if (astart)
				{
					start();
				}
				else
				{
					finish();
				}
			}
			else
			{
				int64_t ltime = ttab_calendar::data::time(start_utc(), finish_utc());
				int64_t lactivityid = m_activityid;
				wheel_parm lparm
				{
					.m_ms = ltemp * 1000,
					.m_intervalms = nullptr ,
					.m_count = 1,
					.m_fun = [ltime,lactivityid,astart](const wheel_node* anode)
					{
						auto pro = std::make_shared<np_actor_activity>(np_actor_activity
							{
								.m_time = ltime,
								.m_calendarid = -1,
								.m_start = astart,
							});
						pro->m_activityids.push_back(lactivityid);
						actor::static_send_actor(actor_manage_activity::actorid(), nguid::make(), pro);
					}
				};
				twheel::wheel().addtimer(lparm);
			}
		}
	}
}//namespace ngl