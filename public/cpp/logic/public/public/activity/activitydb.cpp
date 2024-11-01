#include "actor_manage_activity.h"
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
			actor_manage_activity* lactor = actor();
			const pbdb::db_activity& ldbactivity = item.second.getconst();
			std::shared_ptr<activity> lactivity = activity::make(ldbactivity.m_calendarid(), 
				item.first, ttab_calendar::data::time(ldbactivity.m_beg(), ldbactivity.m_end()), *this
			);
			lactor->add_activity(item.first, lactivity);
		}
		(*lstream).print("");
	}
}//namespace ngl