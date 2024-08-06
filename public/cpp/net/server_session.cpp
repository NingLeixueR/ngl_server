#include "server_session.h"
#include "ttab_servers.h"
#include "nprotocol.h"
#include "nlog.h"

namespace ngl
{
	std::multimap<i32_serverid, i32_sessionid> server_session::m_server;
	std::multimap<i32_sessionid, i32_serverid> server_session::m_session;
	std::shared_mutex server_session::m_mutex;

	void server_session::add(i32_serverid aserverid, i32_sessionid asession)
	{
		log_warn()->print(
			"connect [{}:{}] [{}]", 
			aserverid, ttab_servers::tab(aserverid)->m_name, asession
		);
		lock_write(m_mutex);
		
		{
			auto itor = m_server.find(aserverid);
			if (itor != m_server.end())
			{
				m_server.erase(itor);
			}
		}
		{
			auto itor = m_session.find(asession);
			if (itor != m_session.end())
			{
				m_session.erase(itor);
			}
		}
		m_server.insert(std::make_pair(aserverid, asession));
		m_session.insert(std::make_pair(asession, aserverid));
	}

	void server_session::remove(i32_sessionid asession)
	{
		lock_write(m_mutex);
		auto itor = m_session.find(asession);
		if (itor == m_session.end())
			return;
		m_server.erase(itor->second);
		m_session.erase(itor);
	}

	i32_sessionid server_session::sessionid(i32_serverid aserverid)
	{
		lock_read(m_mutex);
		auto itor = m_server.find(aserverid);
		if (itor == m_server.end())
			return -1;
		return itor->second;
	}

	i32_serverid server_session::serverid(i32_sessionid asessionid)
	{
		lock_read(m_mutex);
		auto itor = m_session.find(asessionid);
		if (itor == m_session.end())
			return -1;
		return itor->second;
	}
}//namespace ngl