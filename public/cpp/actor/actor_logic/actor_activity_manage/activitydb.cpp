#include "actor_activity_manage.h"
#include "activitydb.h"
#include "activity.h"

namespace ngl
{
	void activitydb::initdata()
	{
		log_error()->print("activitydb###loaddb_finish {}", data());
	}
}//namespace ngl