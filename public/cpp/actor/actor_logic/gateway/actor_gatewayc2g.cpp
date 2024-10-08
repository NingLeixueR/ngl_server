﻿#include "actor_gatewayc2g.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_gatewayc2g::actor_gatewayc2g() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_C2G,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = nconfig::m_nodeid,
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	actor_gatewayc2g::~actor_gatewayc2g() {}

	void actor_gatewayc2g::nregister()
	{
		register_handle_custom<actor_gatewayc2g>::func<
			np_actor_gatewayinfo_updata
		>(false);
		// Client 2 Game
		nforward::c2g();
	}

	bool actor_gatewayc2g::handle(const message<np_actor_gatewayinfo_updata>& adata)
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