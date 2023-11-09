#include "actor_switchprocess.h"


namespace ngl
{
	void actor_switchprocess::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_switchprocess>(false
			, null<actor_switch_process<actor_switch_process_role>>
		);
	}


	actor_switchprocess::actor_switchprocess() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_SPROCESS,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = nconfig::m_nodeid,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	actor_switchprocess::~actor_switchprocess() {}
}