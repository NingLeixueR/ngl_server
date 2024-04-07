#pragma once

#include <map>
#include <shared_mutex>

#include "type.h"

namespace ngl
{
	class server_session
	{
		static std::map<i32_serverid, i32_sessionid>	m_server;
		static std::map<i32_sessionid, i32_serverid>	m_session;
		static std::shared_mutex						m_mutex;
	public:
		static void add(i32_serverid aserverid, i32_sessionid asession);
		static void remove(i32_sessionid asession);

		static i32_sessionid get_sessionid(i32_serverid aserverid);
		static i32_serverid get_serverid(i32_sessionid asessionid);
	};
}