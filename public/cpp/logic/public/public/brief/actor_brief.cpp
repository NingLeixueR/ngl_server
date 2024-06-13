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
		register_handle_custom<actor_brief>::func<
			np_actor_roleinfo
		>(false);
	}
}//namespace ngl