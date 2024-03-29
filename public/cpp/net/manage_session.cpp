#include "manage_session.h"

namespace ngl
{
	std::map<i32_serverid, i32_sessionid> manage_session::m_server;
	std::map<i32_sessionid, i32_serverid> manage_session::m_session;
	std::shared_mutex manage_session::m_mutex;
}