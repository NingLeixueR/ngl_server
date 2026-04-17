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
// File overview: Server-side session that binds a network connection to an actor for message dispatch.

#pragma once

#include "tools/type.h"

#include <unordered_map>
#include <shared_mutex>
#include <map>

namespace ngl
{
	class server_session
	{
		server_session() = delete;
		server_session(const server_session&) = delete;
		server_session& operator=(const server_session&) = delete;

		// Maintain both lookup directions because routing frequently starts from either id.
		static std::unordered_map<i32_serverid, i32_sessionid>	m_server;
		static std::unordered_map<i32_sessionid, i32_serverid>	m_session;
		static std::shared_mutex								m_mutex;
	public:
		// Register or replace the session bound to a remote server id.
		static void add(i32_serverid aserverid, i32_sessionid asession);

		// Remove the mapping for a disconnected session.
		static void remove(i32_sessionid asession);

		// Resolve the active session id for a remote server.
		static i32_sessionid sessionid(i32_serverid aserverid);

		// Resolve the remote server id for an active session.
		static i32_serverid serverid(i32_sessionid asessionid);

		// Resolve a human-readable server name from the tab_servers table.
		static bool serverinfo(i32_serverid aserverid, str_servername& asername);

		// Resolve both the session owner and its readable name in one call.
		static bool serverinfobysession(i32_sessionid asessionid, std::pair<str_servername, i32_serverid>& apair);
	};
}//namespace ngl
