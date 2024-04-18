#include "actor_manage_activity.h"
#include "ttab_calendar.h"
#include "activitydb.h"
#include "activity.h"

namespace ngl
{
	void activitydb::initdata()
	{
		LogLocalError("actor_manage_activity###loaddb_finish");
		for (std::pair<const nguid, data_modified<pbdb::db_activity>>& item : data())
		{
			std::cout << "#####################" << std::endl;
			std::cout << "activityid:" << item.first << std::endl;
			actor_manage_activity* lactor = actor();

			
			std::shared_ptr<activity> lactivity = activity::make(item.second.getconst().m_calendarid()
				, item.first
				, ttab_calendar::data::time(item.second.getconst().m_beg(), item.second.getconst().m_end())
				, *this
			);
			lactor->add_activity(item.first, lactivity);
		}
	}
}