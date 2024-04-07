#include "server_session.h"
#include "ttab_servers.h"
#include "nlog.h"

namespace ngl
{

	std::map<i32_serverid, i32_sessionid> server_session::m_server;
	std::map<i32_sessionid, i32_serverid> server_session::m_session;
	std::shared_mutex server_session::m_mutex;

	void server_session::add(i32_serverid aserverid, i32_sessionid asession)
	{
		LogLocalWarn("connect [%:%] [%]", aserverid, ttab_servers::tab(aserverid)->m_name, asession);
		lock_write(m_mutex);
		m_server[aserverid] = asession;
		m_session[asession] = aserverid;
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

	i32_sessionid server_session::get_sessionid(i32_serverid aserverid)
	{
		lock_read(m_mutex);
		i32_sessionid* lpsessionid = tools::findmap(m_server, aserverid);
		if (lpsessionid == nullptr)
			return -1;
		return *lpsessionid;
	}

	i32_serverid server_session::get_serverid(i32_sessionid asessionid)
	{
		lock_read(m_mutex);
		i32_serverid* lpserverid = tools::findmap(m_session, asessionid);
		if (lpserverid == nullptr)
			return -1;
		return *lpserverid;
	}
}