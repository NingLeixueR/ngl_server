#include "actor_manage_activity.h"
#include "ttab_calendar.h"
#include "activitydb.h"
#include "activity.h"

namespace ngl
{
	void activitydb::initdata()
	{
		LogLocalStreamError(lstream);
		lstream << "actor_manage_activity###loaddb_finish" << std::endl;
		for (std::pair<const nguid, data_modified<pbdb::db_activity>>& item : data())
		{
			lstream << "#####################" << std::endl;
			lstream << "activityid:" << item.first << std::endl;
			actor_manage_activity* lactor = actor();
			std::shared_ptr<activity> lactivity = activity::make(item.second.getconst().m_calendarid()
				, item.first
				, ttab_calendar::data::time(item.second.getconst().m_beg(), item.second.getconst().m_end())
				, *this
			);
			lactor->add_activity(item.first, lactivity);
		}
		lstream.print();
	}
}//namespace ngl