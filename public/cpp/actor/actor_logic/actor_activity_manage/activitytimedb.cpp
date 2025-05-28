#include "actor_activity_manage.h"
#include "activitytimedb.h"
#include "ttab_calendar.h"
#include "activity.h"

namespace ngl
{
	void activitytimedb::initdata()
	{
		log_error()->print("activitytimedb###loaddb_finish {}", data());

		for (std::pair<const nguid, data_modified<pbdb::db_activitytimes>>& item : data())
		{
			actor_activity_manage* lactor = actor();
			const pbdb::db_activitytimes& ldbactivitytimes = item.second.getconst();
			int32_t lactivityid = (int32_t)item.first.id();
			std::shared_ptr<activity> lactivity = activity::make(
				lactivityid, ttab_calendar::data::time(ldbactivitytimes.m_beg(), ldbactivitytimes.m_end()), lactor->m_activitydb, lactor->m_activitytimedb
			);
			lactor->add_activity(lactivity);
		}
	}
}//namespace ngl