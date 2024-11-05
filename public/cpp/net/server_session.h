#pragma once

#include "type.h"

#include <shared_mutex>
#include <map>

namespace ngl
{
	class server_session
	{
		server_session() = delete;
		server_session(const server_session&) = delete;
		server_session& operator=(const server_session&) = delete;

		static std::multimap<i32_serverid, i32_sessionid>	m_server;
		static std::multimap<i32_sessionid, i32_serverid>	m_session;
		static std::shared_mutex							m_mutex;
	public:
		static void				add(i32_serverid aserverid, i32_sessionid asession);
		static void				remove(i32_sessionid asession);
		static i32_sessionid	sessionid(i32_serverid aserverid);
		static i32_serverid		serverid(i32_sessionid asessionid);
		static bool				serverinfo(i32_serverid aserverid, str_servername& asername);
		static bool				serverinfobysession(i32_sessionid asessionid, std::pair<str_servername, i32_serverid>& apair);
	};
}//namespace ngl