#pragma once

#include "threadtools.h"
#include "tools.h"
#include "type.h"

namespace ngl
{
	class manage_session
	{
		static std::map<i32_serverid, i32_sessionid> m_server;
		static std::map<i32_sessionid, i32_serverid> m_session;
		static std::shared_mutex		m_mutex;
	public:
		static void add(i32_serverid aserverid, i32_sessionid asession)
		{
			lock_write(m_mutex);
			m_server[aserverid] = asession;
			m_session[asession] = aserverid;
		}

		static i32_sessionid get_sessionid(i32_serverid aserverid)
		{
			lock_read(m_mutex);
			i32_sessionid* lpsessionid = tools::findmap(m_server, aserverid);
			if (lpsessionid == nullptr)
				return -1;
			return *lpsessionid;
		}

		static i32_serverid get_serverid(i32_sessionid asessionid)
		{
			lock_read(m_mutex);
			i32_serverid* lpserverid = tools::findmap(m_session, asessionid);
			if (lpserverid == nullptr)
				return -1;
			return *lpserverid;
		}

	};
	


}