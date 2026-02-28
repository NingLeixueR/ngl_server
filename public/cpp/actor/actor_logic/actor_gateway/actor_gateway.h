/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "actor/actor_logic/actor_gateway/gateway_info.h"
#include "actor/actor_base/actor_manage.h"

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

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// # 通知actor_server [actorid]->[gateway server id]
		void sync_actorserver_gatewayid(const nguid& aguid, bool aisremove);

		void update_gateway_info(const std::shared_ptr<np_actor_gatewayinfo_updata>& ap);

		void session_close(gateway_socket* ainfo);

		bool timer_handle(const message<np_timerparm>& adata);

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