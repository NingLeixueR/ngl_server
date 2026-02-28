/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/

#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_servers.h"
#include "net/server_session.h"
#include "tools/log/nlog.h"

namespace ngl
{
	std::unordered_map<i32_serverid, i32_sessionid> server_session::m_server;
	std::unordered_map<i32_sessionid, i32_serverid> server_session::m_session;
	std::shared_mutex server_session::m_mutex;

	void server_session::add(i32_serverid aserverid, i32_sessionid asession)
	{
		{
			lock_write(m_mutex);
			m_server.erase(aserverid);
			m_session.erase(asession);
			m_server.insert(std::make_pair(aserverid, asession));
			m_session.insert(std::make_pair(asession, aserverid));
		}
		if (auto tab = ttab_servers::instance().tab(nnodeid::tid(aserverid)); tab != nullptr)
		{
			log_error()->print("server_session::add [{}:{}_{}]", nnodeid::tid(aserverid), tab->m_name, nnodeid::tcount(aserverid));
		}
	}

	void server_session::remove(i32_sessionid asession)
	{
		i32_serverid lserverid = 0;
		{
			lock_write(m_mutex);
			if (tools::erasemap(m_session, asession, lserverid))
			{
				m_server.erase(lserverid);
			}
		}
		if (auto tab = ttab_servers::instance().tab(nnodeid::tid(lserverid)); tab != nullptr)
		{
			log_error()->print("server_session::remove [{}:{}_{}]", nnodeid::tid(lserverid), tab->m_name, nnodeid::tcount(lserverid));
		}
	}

	i32_sessionid server_session::sessionid(i32_serverid aserverid)
	{
		lock_read(m_mutex);
		auto lpsession = tools::findmap(m_server, aserverid);
		if (lpsession == nullptr)
		{
			return -1;
		}
		return *lpsession;
	}

	i32_serverid server_session::serverid(i32_sessionid asessionid)
	{
		lock_read(m_mutex);
		auto lpserver = tools::findmap(m_session, asessionid);
		if (lpserver == nullptr)
		{
			return -1;
		}
		return *lpserver;
	}

	bool server_session::serverinfo(i32_serverid aserverid, str_servername& asername)
	{
		if (aserverid != -1)
		{
			if (const tab_servers* tab = ttab_servers::instance().tab(nnodeid::tid(aserverid)); tab != nullptr)
			{
				asername = tab->m_name;
				return true;
			}
		}
		return false;
	}

	bool server_session::serverinfobysession(i32_sessionid asessionid, std::pair<str_servername, i32_serverid>& apair)
	{
		apair.second = serverid(asessionid);
		if (apair.second == -1)
		{
			return false;
		}
		if (serverinfo(apair.second, apair.first) == false)
		{
			return false;
		}
		return true;
	}
}//namespace ngl