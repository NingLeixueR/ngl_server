#include "actor_kcp.h"
#include "ukcp.h"

namespace ngl
{
	actor_kcp::actor_kcp():
		actor(
		actorparm
		{
			.m_parm
			{
				.m_type = ACTOR_KCP,
				.m_area = tab_self_area,
				.m_id = nconfig::m_nodeid,
			},
			.m_weight = 0x7fffffff
		})
	{

	}

	void actor_kcp::nregister()
	{
		register_handle_custom<actor_kcp>::func<
			np_actor_kcp
		>(false);
	}
}//namespace ngl