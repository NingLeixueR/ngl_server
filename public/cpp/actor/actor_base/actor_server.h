#pragma once

#include "actor_manage.h"
#include "naddress.h"
#include "xmlnode.h"
#include "net.h"

namespace ngl
{
	class actor_server : public actor
	{
		actor_server();
	public:
		friend class actor_instance<actor_server>;
		static actor_server& getInstance()
		{
			return actor_instance<actor_server>::instance();
		}

		virtual ~actor_server();

		static void nregister();

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_SERVER, ttab_servers::tab()->m_area, nguid::none_actordataid());
		}

		// ## [actor client -> actor server] register
		// 注册结点
		bool handle(message<np_actornode_register>& adata);

		// 更新结点中的actor
		bool handle(message<np_actornode_update>& adata);

		// 更新gateway表 actor_role.guidid与gateway server id对应关系
		bool handle(message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl
