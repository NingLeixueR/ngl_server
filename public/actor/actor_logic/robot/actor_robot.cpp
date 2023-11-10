#include "actor_robot.h"
#include "actor_register.h"
#include "gameclient_forward.h"

namespace ngl
{

	void actor_robot::actor_register()
	{
		gameclient_forward::g2c();
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