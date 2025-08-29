#pragma once

#include "gateway_info.h"
#include "actor_manage.h"
#include "actor_role.h"
#include "ndbclient.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_gateway : 
		public actor
	{
		actor_gateway(const actor_gateway&) = delete;
		actor_gateway& operator=(const actor_gateway&) = delete;

		gateway_info m_info;

		actor_gateway();
	public:
		friend class actor_instance<actor_gateway>;
		static actor_gateway& instance() 
		{ 
			return actor_instance<actor_gateway>::instance(); 
		}

		virtual ~actor_gateway() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i32_actordataid aactordataid);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		/*template <typename T>
		void sendrole(i32_serverid agameid, i64_actorid arole, const T& adata)
		{
			np_actor_forward<T, forward_c2g> ltemp;
			ltemp.set_data(std::make_shared<T>(adata));
			ltemp.m_uid.push_back(nguid::actordataid(arole));
			ltemp.m_area.push_back(nguid::area(arole));
			nets::sendbyserver(agameid, ltemp, arole, id_guid());
		}*/

		// # 通知actor_server [actorid]->[gateway server id]
		void sync_actorserver_gatewayid(const nguid& aguid, bool aisremove);

		void update_gateway_info(const std::shared_ptr<np_actor_gatewayinfo_updata>& ap);

		void session_close(gateway_socket* ainfo);

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);

		bool handle(const message<np_gateway_close_session>& adata);

		// # actor_login通知gateway玩家已经登录(并纪录session及对应的game服务器)
		bool handle(const message<np_actorrole_login>& adata);

		// # 获取kcp-session
		bool handle(const message<np_actor_kcp>& adata);

		// # 玩家切换game进程
		bool handle(const message<np_actorswitch_process<np_actorswitch_process_role>>& adata);

		// # 断开连接
		bool handle(const message<np_actor_session_close>& adata);
		
		// # 玩家登陆
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);

		// # 获取kcp-session
		bool handle(const message<pbnet::PROBUFF_NET_KCPSESSION>& adata);
	};
}//namespace ngl

