#include "actor_plays.h"
#include "matching.h"

namespace ngl
{
	actor_plays::actor_plays(int32_t adataid) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_PLAYS,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = adataid
				},
				.m_weight = 10,
			})
	{
		int32_t tid = roomid::tid(adataid);
		m_tab = allcsv::tab<tab_plays>(tid);		
	}

	void actor_plays::actor_register()
	{
		// ¶¨Ê±Æ÷
		register_timer<actor_plays>(&actor_plays::timer_handle);
	}
}