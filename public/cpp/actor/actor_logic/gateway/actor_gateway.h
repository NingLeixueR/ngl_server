#pragma once

#include "actor_manage.h"
#include "actor_role.h"
#include "gateway_info.h"
#include "ndbclient.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_gateway : public actor
	{
		actor_gateway();
	public:
		friend class actor_instance<actor_gateway>;
		static actor_gateway& getInstance() 
		{ 
			return actor_instance<actor_gateway>::instance(); 
		}

		virtual ~actor_gateway();

		static void nregister();

		gateway_info m_info;

		// ## 通知actor_server [actorid]->[gateway server id]
		void sync_actorserver_gatewayid(const nguid& aguid, bool aisremove);

		void update_gateway_info(actor_gateway_info_updata* ap);

		void session_close(gateway_socket* ainfo);
		// actor_login通知gateway玩家已经登录(并纪录session及对应的game服务器)
		bool handle(message<actor_role_login>& adata);
		// 玩家登陆
		bool handle(message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);
		// 获取kcp-session
		bool handle(message<actor_protocol_kcp>& adata);
		bool handle(message<pbnet::PROBUFF_NET_KCPSESSION>& adata);		
		// 玩家切换game进程
		bool handle(message<actor_switch_process<actor_switch_process_role>>& adata);
		// 断开连接
		bool handle(message<actor_session_close>& adata);
	};
}//namespace ngl

