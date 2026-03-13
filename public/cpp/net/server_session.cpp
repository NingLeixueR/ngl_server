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
	namespace
	{
		constexpr std::size_t G_SERVER_SESSION_RESERVE = 64;
	}

	std::unordered_map<i32_serverid, i32_sessionid> server_session::m_server;
	std::unordered_map<i32_sessionid, i32_serverid> server_session::m_session;
	std::shared_mutex server_session::m_mutex;

	void server_session::add(i32_serverid aserverid, i32_sessionid asession)
	{
		{
			lock_write(m_mutex);
			if (m_server.empty())
			{
				m_server.reserve(G_SERVER_SESSION_RESERVE);
				m_session.reserve(G_SERVER_SESSION_RESERVE);
			}

			if (const auto lserver_it = m_server.find(aserverid); lserver_it != m_server.end())
			{
				m_session.erase(lserver_it->second);
				m_server.erase(lserver_it);
			}

			if (const auto lsession_it = m_session.find(asession); lsession_it != m_session.end())
			{
				m_server.erase(lsession_it->second);
				m_session.erase(lsession_it);
			}

			m_server.emplace(aserverid, asession);
			m_session.emplace(asession, aserverid);
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
			if (const auto lsession_it = m_session.find(asession); lsession_it != m_session.end())
			{
				lserverid = lsession_it->second;
				m_server.erase(lserverid);
				m_session.erase(lsession_it);
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
		const auto liter = m_server.find(aserverid);
		if (liter == m_server.end())
		{
			return -1;
		}
		return liter->second;
	}

	i32_serverid server_session::serverid(i32_sessionid asessionid)
	{
		lock_read(m_mutex);
		const auto liter = m_session.find(asessionid);
		if (liter == m_session.end())
		{
			return -1;
		}
		return liter->second;
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
