#include "calendar.h"


namespace ngl
{
	std::map<ENUM_ACTOR, std::set<ECalendarType>> calendar::m_actortypeofcalendartype =
	{
		{ACTOR_ACTIVITY_MANAGE, {ECalendarTypeActivity}},
	};


}//namespace ngl