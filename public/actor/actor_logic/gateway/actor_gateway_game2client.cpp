#include "actor_gateway_game2client.h"
#include "actor_register.h"
#include "gateway_game_forward.h"

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

	void actor_gateway_game2client::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gateway_game2client>(
			false
			, null<actor_gateway_info_updata>
			);
		// Game 2 Client
		gateway_game_forward::game2client();
	}
}