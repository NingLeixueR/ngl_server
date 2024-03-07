#include "actor_gateway_game2client.h"
#include "nregister.h"
#include "gameclient_forward.h"

namespace ngl
{
	actor_gateway_game2client::actor_gateway_game2client() :
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

	actor_gateway_game2client::~actor_gateway_game2client() {}

	void actor_gateway_game2client::nregister()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gateway_game2client>(
			false
			, dregister_fun_handle(actor_gateway_game2client, actor_gateway_info_updata)
		);
		// Game 2 Client
		gameclient_forward::g2c();
	}

	bool actor_gateway_game2client::handle(message<actor_gateway_info_updata>& adata)
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