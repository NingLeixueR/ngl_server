/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Route actor for server-type nodes, handles cross-node message forwarding and actor discovery.

#pragma once

#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/naddress.h"
#include "net/tcp/ntcp.h"

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

		// Respond to a node registration request and forward it to other known nodes.
		void reply_register(const pack* apack, i32_serverid aserverid);
		void forward_register(const pack* apack, const nactornode& anode, i32_serverid aserverid, const std::vector<i64_actorid>& aadd);

		// Handle: actor_client registers its node with this actor_server.
		bool handle([[maybe_unused]] const message<np_actornode_register>& adata);

		// Handle: update the actor set hosted on a remote node.
		bool handle([[maybe_unused]] const message<np_actornode_update_server>& adata);

		// Handle: batch node-info update (with optional completion callback).
		bool handle([[maybe_unused]] const message<np_actornode_update_mass>& adata);

		// Handle: synchronize actor_role guid to gateway server id mapping.
		bool handle([[maybe_unused]] const message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl