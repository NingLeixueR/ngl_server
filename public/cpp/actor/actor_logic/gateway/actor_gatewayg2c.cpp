#include "actor_gatewayg2c.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_gatewayg2c::actor_gatewayg2c() :
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

	actor_gatewayg2c::~actor_gatewayg2c() {}

	void actor_gatewayg2c::nregister()
	{
		register_handle_custom<actor_gatewayg2c>::func<
			np_actor_gatewayinfo_updata
		>(false);
		// Game 2 Client
		nforward::g2c();
	}

	bool actor_gatewayg2c::handle(const message<np_actor_gatewayinfo_updata>& adata)
	{
		m_info.gatewayinfo_updata(*adata.get_data());
		return true;
	}
}//namespace ngl