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
// File overview: Declares interfaces for actor client.

#pragma once

#include "actor/actor_base/core/naddress.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/actor.h"
#include <unordered_map>

namespace ngl
{
	//####################################################################
	// # Need to actor_clientoractor_server
	// # Area inpack actor_server
	// # All pack actor_client
	// # Actor_client used to savenguidandserverid corresponding,
	// # Actor_client actor, actor
	// # Actor_serverresponsible for actor_clientinlocal nguidcorresponding
	//###################################################################
	class actor_client : 
		public actor
	{
		actor_client(const actor_client&) = delete;
		actor_client& operator=(const actor_client&) = delete;

		using funclist = std::list<std::function<void()>>; 
		std::unordered_map<i32_serverid, funclist>	m_connectfun;		// Connection[specifiedserver]successfulafterexecute
		std::set<i32_serverid>				m_connectserverid;		// Connection serverid

		actor_client();
		virtual ~actor_client() = default;
	public:
		friend class actor_instance<actor_client>;
		static actor_client& instance() 
		{ 
			return actor_instance<actor_client>::instance(); 
		}
		
		// # Registerneed tohandle message
		static void nregister();
	private:
		// # Toactor_serverregisteractor_client
		void actor_server_register(i32_serverid aserverid);

		// # Setconnectionaftertime for exampleactor_dbclient registerconnectionafterevent(loaddata)
		void set_connect_fnish(i32_serverid aserverid);

		// # Connectionsuccessfulafter
		void connect_fnish();

		// # Whetherneed to connection
		bool isactiv_connect(i32_serverid aserverid);

		// # Connection
		void activ_connect(i32_serverid aserverid);
	public:
		// # Toactor_serverregisternode
		void actor_server_register();

		// # Actor_client actorid
		static i64_actorid actorid();

		// # Netconnectionactor serversuccessful
		bool handle(const message<np_connect_actor_server>& adata);

		// # Registernode
		bool handle(const message<np_actor_server_register>& adata);

		// # Actor_serverresponseregisternode
		bool handle(const message<np_actornode_register_response>& adata);

		// # Actorclient connection
		bool handle(const message<np_actorclient_node_connect>& adata);

		// # Servertoactorclientsynchronizenodeinfo
		bool handle(const message<np_actornode_update>& adata);

		// # Toactorclientsynchronizenodeinfo( )
		bool handle(const message<np_actornode_update_mass>& adata);

		// # Connectionsuccessfulafterexecutetask
		bool handle(const message<np_actornode_connect_task>& adata);

		// # Synchronizeactor_rolecorresponding gateway
		bool handle(const message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl