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
// File overview: Route actor for client-type nodes, handles cross-node message forwarding and actor discovery.

#pragma once

#include "actor/actor_base/core/naddress.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/actor.h"
#include <unordered_map>

namespace ngl
{
	// Route actor for client-type nodes.
	// Each node runs either actor_client or actor_server depending on its role.
	// actor_server is used by area/zone nodes; actor_client is used by all others.
	// actor_client stores the mapping between nguid and remote server id,
	// and forwards messages to actor_server on the target node for final delivery.
	class actor_client :
		public actor
	{
		actor_client(const actor_client&) = delete;
		actor_client& operator=(const actor_client&) = delete;

		using funclist = std::list<std::function<void()>>;
		std::unordered_map<i32_serverid, funclist>	m_connectfun;		// Callbacks to run after connecting to a specific server.
		std::set<i32_serverid>				m_connectserverid;		// Server ids with established connections.

		actor_client();
		virtual ~actor_client() = default;
	public:
		friend class actor_instance<actor_client>;
		static actor_client& instance()
		{
			return actor_instance<actor_client>::instance();
		}

		// Register message handlers for this actor.
		static void nregister();
	private:
		// Register this node with the remote actor_server.
		void actor_server_register(i32_serverid aserverid);

		// Run post-connection callbacks (e.g. actor_dbclient triggers data loading).
		void set_connect_fnish(i32_serverid aserverid);

		// Execute all pending post-connection callbacks.
		void connect_fnish();

		// Check whether a connection to the given server is needed.
		bool isactiv_connect(i32_serverid aserverid);

		// Initiate a connection to the given server.
		void activ_connect(i32_serverid aserverid);
	public:
		// Register this node with all known actor_server instances.
		void actor_server_register();

		// Return the singleton actor id for actor_client.
		static i64_actorid actorid();

		// Handle: network connection to an actor_server established.
		bool handle([[maybe_unused]] const message<np_connect_actor_server>& adata);

		// Handle: register this node on the remote actor_server.
		bool handle([[maybe_unused]] const message<np_actor_server_register>& adata);

		// Handle: actor_server responds to our node registration.
		bool handle([[maybe_unused]] const message<np_actornode_register_response>& adata);

		// Handle: actor_client node connection request.
		bool handle([[maybe_unused]] const message<np_actorclient_node_connect>& adata);

		// Handle: actor_server pushes updated node info to this client.
		bool handle([[maybe_unused]] const message<np_actornode_update>& adata);

		// Handle: batch node-info update (with optional completion callback).
		bool handle([[maybe_unused]] const message<np_actornode_update_mass>& adata);

		// Handle: run deferred tasks after a connection is fully established.
		bool handle([[maybe_unused]] const message<np_actornode_connect_task>& adata);

		// Handle: synchronize actor_role-to-gateway mapping.
		bool handle([[maybe_unused]] const message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl