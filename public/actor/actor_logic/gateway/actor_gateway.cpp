#include "actor_gateway.h"
#include "actor_register.h"

namespace ngl
{
	void actor_gateway::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gateway>(
			false
			, null<actor_role_login>
			, null<actor_switch_process<actor_switch_process_role>>
			, null<actor_session_close>
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_gateway>(
			false
			, null<pbnet::PROBUFF_NET_ROLE_LOGIN>
		);
	}
}