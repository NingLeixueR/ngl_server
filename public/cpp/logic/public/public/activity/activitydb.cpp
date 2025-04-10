#include "actor_activity_manage.h"
#include "ttab_calendar.h"
#include "activitydb.h"
#include "activity.h"

namespace ngl
{
	void activitydb::initdata()
	{
		auto lstream = log_error();
		(*lstream) << "actor_manage_activity###loaddb_finish" << std::endl;
		for (std::pair<const nguid, data_modified<pbdb::db_activity>>& item : data())
		{
			(*lstream) << std::format("activityid:{}", item.first) << std::endl;
			actor_activity_manage* lactor = actor();
			const pbdb::db_activity& ldbactivity = item.second.getconst();
			int32_t lactivityid = (int32_t)item.first.id();
			std::shared_ptr<activity> lactivity = activity::make(ldbactivity.m_calendarid(), 
				lactivityid, ttab_calendar::data::time(ldbactivity.m_beg(), ldbactivity.m_end()), *this
			);
			lactor->add_activity(item.first, lactivity);
		}
		(*lstream).print("");
	}
}//namespace ngl