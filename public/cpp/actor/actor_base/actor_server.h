#pragma once

#include "actor_manage.h"
#include "naddress.h"
#include "net.h"

namespace ngl
{
	class actor_server : 
		public actor
	{
		actor_server(const actor_server&) = delete;
		actor_server& operator=(const actor_server&) = delete;

		actor_server();
	public:
		friend class actor_instance<actor_server>;
		static actor_server& instance()
		{
			return actor_instance<actor_server>::instance();
		}

		virtual ~actor_server() = default;

		static void nregister();

		static i64_actorid actorid();

		void print(const std::vector<i64_actorid>& avec)const;

		// # 回复/转发[np_actornode_register]请求
		void reply_np_actornode_register(const pack* apack, i32_serverid aserverid);
		void forward_np_actornode_register(
			const pack* apack, const nactornode& anode, i32_serverid aserverid, const std::vector<i64_actorid>& aadd
		);

		// # [actor client -> actor server] register
		// # 注册结点
		bool handle(const message<np_actornode_register>& adata);

		// # 更新结点中的actor
		bool handle(const message<np_actornode_update_server>& adata);

		//# 向actor客户端同步结点信�?群发)
		bool handle(const message<np_actornode_update_mass>& adata);

		// # 更新gateway�?actor_role.guidid与gateway server id对应关系
		bool handle(const message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl