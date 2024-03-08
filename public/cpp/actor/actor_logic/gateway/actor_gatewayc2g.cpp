#include "gameclient_forward.h"
#include "actor_gatewayc2g.h"
#include "nregister.h"

namespace ngl
{
	actor_gatewayc2g::actor_gatewayc2g() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_CLIENT2GAME,
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
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gatewayc2g>(
			false
			, dregister_fun_handle(actor_gatewayc2g, actor_gateway_info_updata)
		);
		// Client 2 Game
		gameclient_forward::c2g();
	}

	bool actor_gatewayc2g::handle(message<actor_gateway_info_updata>& adata)
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
}