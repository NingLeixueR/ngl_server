#include "actor_create.h"

namespace ngl
{
	void actor_create::nregister()
	{
		register_handle_custom<actor_create>::func<
			np_actorswitch_process<np_actorswitch_process_role>
		>(false);
	}

	i64_actorid actor_create::actorid(i32_actordataid adataid)
	{
		return nguid::make(ACTOR_CREATE, ttab_servers::tab()->m_area, adataid);
	}

	actor_create::actor_create() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CREATE,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = nconfig::m_nodeid,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}
}//namespace ngl