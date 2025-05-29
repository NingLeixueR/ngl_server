#include "actor_activity_manage.h"
#include "activitytimedb.h"
#include "ttab_calendar.h"
#include "activity.h"

namespace ngl
{
	void activitytimedb::initdata()
	{
		log_error()->print("activitytimedb###loaddb_finish {}", data());
	}
}//namespace ngl