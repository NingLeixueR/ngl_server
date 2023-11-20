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
			false
			, dregister_fun_handle(actor_manage_robot, robot_pram)
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_manage_robot>(
			false
			, dregister_fun_handle(actor_manage_robot, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE)
		);
	}

}