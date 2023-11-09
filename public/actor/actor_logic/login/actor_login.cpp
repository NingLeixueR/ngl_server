#include "actor_login.h"
#include "actor_register.h"

namespace ngl
{
	void actor_login::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_login>(false
			, null<pbnet::PROBUFF_NET_ACOUNT_LOGIN>
			);

		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_login>(false
			, null<actor_disconnect_close>
		);

		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_login>(true
			, null<actor_server_connect>
		);
	}

	
}