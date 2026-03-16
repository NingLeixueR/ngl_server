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
// File overview: Declares interfaces for net.

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

		static std::unordered_map<i32_serverid, i32_sessionid>	m_server;	// key:serverid value:sessionid
		static std::unordered_map<i32_sessionid, i32_serverid>	m_session;	// key:sessionid value:serverid
		static std::shared_mutex								m_mutex;
	public:
		// # Addserveridandsessionid
		static void add(i32_serverid aserverid, i32_sessionid asession);

		// # Removesessionid
		static void remove(i32_sessionid asession);

		// # Serveridgetsessionid
		static i32_sessionid sessionid(i32_serverid aserverid);

		// # Sessionidgetserverid
		static i32_serverid serverid(i32_sessionid asessionid);

		// # Serveridgettab_serverstableinservername
		static bool serverinfo(i32_serverid aserverid, str_servername& asername);

		// # Sessiongetserveridandservername
		static bool serverinfobysession(i32_sessionid asessionid, std::pair<str_servername, i32_serverid>& apair);
	};
}//namespace ngl