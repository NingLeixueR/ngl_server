#include "actor_brief.h"
#include "net.pb.h"

namespace ngl
{
	actor_brief::actor_brief() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_BRIEF,
						.m_area = ttab_servers::tab()->m_area
					},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_brief::nregister()
	{
		// Ð­Òé×¢²á
		register_actor_handle<EPROTOCOL_TYPE_CUSTOM
			, actor_brief
			, np_actor_roleinfo
		>(false);
	}
}//namespace ngl