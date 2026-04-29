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
// File overview: Cross-node address table mapping actor guids to server sessions.

#pragma once

#include "actor/actor_base/core/handle_pram.h"
#include "actor/actor_base/core/nguid.h"
#include "tools/type.h"

#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

namespace ngl
{
	// Global routing table shared by actor_client / actor_server to resolve
	// actor ids, server sessions, and role-to-gateway ownership.
	class naddress
	{
		naddress() = delete;
		naddress(const naddress&) = delete;
		naddress& operator=(const naddress&) = delete;
	public:
		// Full actor guid -> owning server id.
		using map_guidserver		= std::unordered_map<nguid, i32_serverid>;
		// Actor type -> all known actor guids of that type.
		using map_typeguid			= std::unordered_map<i16_actortype, std::unordered_set<nguid>>;
		// Server id -> live node/session metadata.
		using map_servernode		= std::unordered_map<i32_serverid, nnode_session>;
		// Role actor guid -> gateway server currently serving that role.
		using map_rolegateway		= std::unordered_map<nguid, i32_serverid>;
		// Snapshot-style callback that can inspect all routing tables together.
		using ergodic_callbackfun	= std::function<bool(const map_guidserver&, const map_servernode&, const map_rolegateway&)>;
		// Iteration callback over known server/session entries.
		using foreach_callbackfun	= std::function<bool(const nnode_session&)>;

		static naddress::map_guidserver		m_actorserver;
		static naddress::map_typeguid		m_actortypeserver;
		static naddress::map_servernode		m_session;
		static naddress::map_rolegateway	m_rolegateway;
		static std::shared_mutex			m_actor_mutex;		// Protects m_actorserver + m_actortypeserver
		static std::shared_mutex			m_session_mutex;	// Protects m_session
		static std::shared_mutex			m_gateway_mutex;	// Protects m_rolegateway

		// Debug helpers for the actor -> server routing table.
		static void print_address();
		static void print_address(const char* ainfo, i32_serverid aserverid, const nguid& aguid);
	public:
#pragma region base
		// Register or update one node entry.
		static bool set_node(const nactornode& anode);

	private:
		// Lock-free helper used only while m_mutex is already held.
		static void nosafe_actor_address_add(i32_serverid aserverid, i64_actorid adataid);

		// Lock-free helper used only while m_mutex is already held.
		static void nosafe_actor_address_del(i64_actorid adataid);

	public:
		// Add one actor -> server mapping.
		static void actor_address_add(i32_serverid aserverid, i64_actorid adataid);

		// Add a batch of actor -> server mappings.
		static void actor_address_add(i32_serverid aserverid, const std::vector<i64_actorid>& avec);

		// Remove one actor -> server mapping.
		static void actor_address_del(i64_actorid adataid);

		// Remove a batch of actor -> server mappings.
		static void actor_address_del(const std::vector<i64_actorid>& avec);

		// Attach the current TCP session id to a known server.
		static void set_session(i32_serverid aserverid, i32_sessionid asession);

		// Resolve the TCP session id for a known server.
		static i32_sessionid sessionid(i32_serverid aserverid);

		// Resolve which server currently owns a specific actor.
		static i32_serverid serverid(const nguid& aguid);

		// Collect every server that currently hosts at least one actor of the given type.
		static void serveridlist(ENUM_ACTOR atype, std::set<i32_serverid>& aservers);
#pragma endregion

#pragma region gateway
		// Track which gateway currently owns the specified role actor.
		static void gatewayid_add(const nguid& aguid, i32_serverid aserverid);

		// Remove the role -> gateway association.
		static void gatewayid_del(const nguid& aguid);

		// Resolve the gateway server that owns the specified role actor.
		static i32_serverid gatewayid(const nguid& aguid);

		// Resolve all distinct gateway servers serving the specified role set.
		static void gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset);
#pragma endregion
		// Iterate over every known server/session entry until the callback returns false.
		static void foreach(const foreach_callbackfun& afun);

		// Visit the three routing tables under a shared read lock.
		static void ergodic(const ergodic_callbackfun& afun);

		// Return a copy of the actor -> server map for lock-free callers.
		static map_guidserver get_actorserver_map();

		// Resolve the gateway session that currently serves a role actor.
		static i32_sessionid sessionbyrole(i16_area aarea, i32_actordataid aroleid);

		// Invoke the forwarding callback embedded in a handle_pram, if present.
		static bool forward(handle_pram& apram);
	};
}//namespace ngl
