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
					.m_type = ACTOR_GATEWAY_GAME2CLIENT,
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
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gatewayg2c>(
			false
			, dregister_fun_handle(actor_gatewayg2c, np_actor_gatewayinfo_updata)
		);
		// Game 2 Client
		nforward::g2c();
	}

	bool actor_gatewayg2c::handle(message<np_actor_gatewayinfo_updata>& adata)
	{
		auto lpram = adata.m_data;
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