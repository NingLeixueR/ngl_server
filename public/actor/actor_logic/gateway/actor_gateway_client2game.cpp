#include "actor_gateway_client2game.h"
#include "actor_register.h"
#include "gateway_game_forward.h"

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

	void actor_gateway_client2game::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gateway_client2game>(
			false
			, null<actor_gateway_info_updata>
			);
		// Client 2 Game
		gateway_game_forward::client2game();
	}
}