#include "actor_gateway_client2game.h"
#include "nregister.h"
#include "gameclient_forward.h"

namespace ngl
{
	actor_gateway_client2game::actor_gateway_client2game() :
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

	actor_gateway_client2game::~actor_gateway_client2game() {}

	void actor_gateway_client2game::nregister()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gateway_client2game>(
			false
			, dregister_fun_handle(actor_gateway_client2game, actor_gateway_info_updata)
		);
		// Client 2 Game
		gameclient_forward::c2g();
	}

	bool actor_gateway_client2game::handle(message<actor_gateway_info_updata>& adata)
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