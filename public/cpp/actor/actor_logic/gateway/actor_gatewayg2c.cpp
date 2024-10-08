﻿#include "actor_gatewayg2c.h"
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
					.m_area = ttab_servers::tab()->m_area,
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
		auto lpram = adata.get_data();
		for (auto& item : lpram->m_delsocket)
		{
			m_info.remove_socket(item);
		}
		for (auto& item : lpram->m_delactorid)
		{
			m_info.remove_actorid(item);
		}
		for (auto& item : lpram->m_add)
		{
			m_info.updata(item);
		}
		return true;
	}
}//namespace ngl