#include "actor_activity_manage.h"
#include "ttab_calendar.h"
#include "activitydb.h"
#include "activity.h"

namespace ngl
{
	void activitydb::initdata()
	{
		std::vector<nguid> lactivityids;
		for (std::pair<const nguid, data_modified<pbdb::db_activity>>& item : data())
		{
			lactivityids.push_back(item.first);
			actor_activity_manage* lactor = actor();
			const pbdb::db_activity& ldbactivity = item.second.getconst();
			int32_t lactivityid = (int32_t)item.first.id();
			std::shared_ptr<activity> lactivity = activity::make(ldbactivity.m_calendarid(), 
				lactivityid, ttab_calendar::data::time(ldbactivity.m_beg(), ldbactivity.m_end()), *this
			);
			lactor->add_activity(item.first, lactivity);
		}
		log_error()->print("actor_manage_activity###loaddb_finish activityids:{}", lactivityids);
	}
}//namespace ngl