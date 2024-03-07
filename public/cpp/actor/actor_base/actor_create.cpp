#include "actor_create.h"

namespace ngl
{
	void actor_create::nregister()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_create>(false
			, (Tfun<actor_create, actor_switch_process<actor_switch_process_role>>) & actor_create::handle
		);
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
}