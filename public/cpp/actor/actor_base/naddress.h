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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/actor_base/handle_pram.h"
#include "actor/actor_base/nguid.h"
#include "tools/type.h"

#include <map>
#include <shared_mutex>

namespace ngl
{
	// # (Actor_client/actor_server) used tomanageactoraddress
	// # (Actor_client/actor_server)
	class naddress
	{
		naddress() = delete;
		naddress(const naddress&) = delete;
		naddress& operator=(const naddress&) = delete;
	public:
		//# [ACTOR_ID] -> SERVER_ID
		//# [ACTOR_TYPE + EARE_ID + DATA_ID] -> SERVER_ID
		using map_guidserver		= std::map<nguid, i32_serverid>;
		//# [ACTOR_TYPE]  -> set<ACTOR_ID>
		//# [ACTOR_TYPE]  -> set<ACTOR_TYPE + EARE_ID + DATA_ID>
		using map_typeguid			= std::map<i16_actortype, std::set<nguid>>;
		//# [SERVER_ID] -> [SESSION_ID]
		using map_servernode		= std::map<i32_serverid, nnode_session>;
		//# [ACTOR_ID] -> [GATEWAY_SERVER_ID]
		using map_rolegateway		= std::map<nguid, i32_serverid>;
		// # [Ergodic]callback
		using ergodic_callbackfun	= std::function<bool(const map_guidserver&, const map_servernode&, const map_rolegateway&)>;
		// # [Foreach]callback
		using foreach_callbackfun	= std::function<bool(const nnode_session&)>;

		static naddress::map_guidserver		m_actorserver;
		static naddress::map_typeguid		m_actortypeserver;
		static naddress::map_servernode		m_session;
		static naddress::map_rolegateway	m_rolegateway;
		static std::shared_mutex			m_mutex;

		// # Debug
		static void print_address();
		static void print_address(const char* ainfo, i32_serverid aserverid, const nguid& aguid);
	public:
#pragma region base
		// # Setnode
		static bool set_node(const nactornode& anode);

		// # Addactor
		static void actor_address_add(i32_serverid aserverid, i64_actorid adataid);

		// # Adda group ofactor
		static void actor_address_add(i32_serverid aserverid, const std::vector<i64_actorid>& avec);

		// # Deleteactor
		static void actor_address_del(i64_actorid adataid);

		// # Deletea group ofactor
		static void actor_address_del(const std::vector<i64_actorid>& avec);

		// # Setsession
		static void set_session(i32_serverid aserverid, i32_sessionid asession);

		// # Getsession
		static i32_sessionid sessionid(i32_serverid aserverid);

		// # Getserver id
		static i32_serverid serverid(const nguid& aguid);

		// # GetENUM_ACTOR corresponding server
		static void serveridlist(ENUM_ACTOR atype, std::set<i32_serverid>& aservers);
#pragma endregion

#pragma region gateway
		// # Actor_role.actoridandgatewayid
		static void gatewayid_add(const nguid& aguid, i32_serverid aserverid);

		// # Actor_role.actoridandgatewayid
		static void gatewayid_del(const nguid& aguid);

		// # [ACTOR_ID(ACTOR_ROLE actor)] -> [GATEWAY_SERVER_ID]
		static i32_serverid gatewayid(const nguid& aguid);

		// # Geta group ofactor_role.actoridandgatewayid
		static void gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset);
#pragma endregion
		// # [Std::function<bool(const nnode_session&)>]
		// # Ifreturn false
		// Translated comment.
		static void foreach(const foreach_callbackfun& afun);

		//# [ergodic_callbackfun:std::function<bool(map_guidserver&, map_servernode&)>]
		static void ergodic(const ergodic_callbackfun& afun);

		// # Getactor->server (thread )
		static map_guidserver get_actorserver_map();

		// # Areaanddataidgetsession
		static i32_sessionid sessionbyrole(i16_area aarea, i32_actordataid aroleid);

		// # Forwarding
		static bool forward(handle_pram& apram);
	};
}//namespace ngl