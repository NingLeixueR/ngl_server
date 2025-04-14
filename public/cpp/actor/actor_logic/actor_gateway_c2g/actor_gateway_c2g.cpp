#include "actor_gateway_c2g.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_gateway_c2g::actor_gateway_c2g() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_C2G,
					.m_area = tab_self_area,
					.m_id = nconfig::m_nodeid,
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	ENUM_ACTOR actor_gateway_c2g::actor_type()
	{
		return ACTOR_GATEWAY_C2G;
	}

	i64_actorid actor_gateway_c2g::actorid(i32_actordataid aactordataid)
	{
		return nguid::make(ACTOR_GATEWAY_C2G, tab_self_area, aactordataid);
	}

	void actor_gateway_c2g::nregister()
	{
		register_handle_custom<actor_gateway_c2g>::func<
			np_actor_gatewayinfo_updata
		>(false);
		// Client 2 Game
		nforward::c2g();
	}
}//namespace ngl