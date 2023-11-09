#include "actor_robot.h"
#include "actor_register.h"
#include "gateway_game_forward.h"

namespace ngl
{

	void actor_robot::actor_register()
	{
		gateway_game_forward::game2client();
	}

	void actor_manage_robot::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_manage_robot>(
			false,
			null<robot_pram>
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_manage_robot>(
			false,
			null<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>
		);
	}

}