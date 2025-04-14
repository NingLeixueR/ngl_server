#include "actor_gateway_g2c.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_gateway_g2c::actor_gateway_g2c() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_G2C,
					.m_area = tab_self_area,
					.m_id = nconfig::m_nodeid,
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	ENUM_ACTOR actor_gateway_g2c::actor_type()
	{
		return ACTOR_GATEWAY_G2C;
	}

	i64_actorid actor_gateway_g2c::actorid(i32_actordataid aactordataid)
	{
		return nguid::make(ACTOR_GATEWAY_G2C, tab_self_area, aactordataid);
	}

	void actor_gateway_g2c::nregister()
	{
		register_handle_custom<actor_gateway_g2c>::func<
			np_actor_gatewayinfo_updata
		>(false);
		// Game 2 Client
		nforward::g2c();
	}
}//namespace ngl