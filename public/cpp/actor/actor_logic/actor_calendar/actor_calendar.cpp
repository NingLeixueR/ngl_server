#include "calendar_function.h"
#include "actor_calendar.h"
#include "ttab_calendar.h"

namespace ngl
{
	actor_calendar::actor_calendar() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CALENDAR,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_calendar::actor_type()
	{
		return ACTOR_CALENDAR;
	}

	i64_actorid actor_calendar::actorid()
	{
		return nguid::make(ACTOR_CALENDAR, tab_self_area, nguid::none_actordataid());
	}

	void actor_calendar::init()
	{
		m_calendar.set(this);
	}

	void actor_calendar::loaddb_finish(bool adbishave) 
	{
	}

	void actor_calendar::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_calendar>::func<
			np_calendar
		>(true);
	}
}// namespace ngl