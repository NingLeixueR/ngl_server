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

		// # 鍥炲/杞彂[np_actornode_register]璇锋眰
		void reply_np_actornode_register(const pack* apack, i32_serverid aserverid);
		void forward_np_actornode_register(
			const pack* apack, const nactornode& anode, i32_serverid aserverid, const std::vector<i64_actorid>& aadd
		);

		// # [actor client -> actor server] register
		// # 娉ㄥ唽缁撶偣
		bool handle(const message<np_actornode_register>& adata);

		// # 鏇存柊缁撶偣涓殑actor
		bool handle(const message<np_actornode_update_server>& adata);

		//# 鍚慳ctor瀹㈡埛绔悓姝ョ粨鐐逛俊鎭?缇ゅ彂)
		bool handle(const message<np_actornode_update_mass>& adata);

		// # 鏇存柊gateway琛?actor_role.guidid涓巊ateway server id瀵瑰簲鍏崇郴
		bool handle(const message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl