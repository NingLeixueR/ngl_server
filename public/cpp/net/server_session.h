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

		static std::multimap<i32_serverid, i32_sessionid>	m_server;	// key:serverid value:sessionid
		static std::multimap<i32_sessionid, i32_serverid>	m_session;	// key:sessionid value:serverid
		static std::shared_mutex							m_mutex;
	public:
		// # 添加服务器id与sessionid映射
		static void				add(i32_serverid aserverid, i32_sessionid asession);

		// # 移除sessionid
		static void				remove(i32_sessionid asession);

		// # 根据服务器id获取sessionid
		static i32_sessionid	sessionid(i32_serverid aserverid);

		// # 根据sessionid获取服务器id
		static i32_serverid		serverid(i32_sessionid asessionid);

		// # 根据serverid获取tab_servers表中的服务器名称
		static bool				serverinfo(i32_serverid aserverid, str_servername& asername);

		// # 根据session获取服务器id与服务器名称
		static bool				serverinfobysession(i32_sessionid asessionid, std::pair<str_servername, i32_serverid>& apair);
	};
}//namespace ngl