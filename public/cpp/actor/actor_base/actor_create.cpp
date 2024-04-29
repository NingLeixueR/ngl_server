#include "actor_create.h"

namespace ngl
{
	void actor_create::nregister()
	{
		type_register_actor_handle<EPROTOCOL_TYPE_CUSTOM, actor_create>::func<
			np_actorswitch_process<np_actorswitch_process_role>
		>(false);
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