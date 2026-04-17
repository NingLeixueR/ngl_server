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
// File overview: Implements server session lifecycle and connection-to-actor binding.


#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_servers.h"
#include "net/server_session.h"
#include "tools/log/nlog.h"

#include <algorithm>

namespace ngl
{
	namespace
	{
		constexpr std::size_t g_sess_resv = 64;

		void grow_maps(
			std::unordered_map<i32_serverid, i32_sessionid>& aserver_map,
			std::unordered_map<i32_sessionid, i32_serverid>& asession_map)
		{
			// Reserve both maps together so the bi-directional update stays cheap and avoids
			// rehash churn during bursts of reconnects.
			const std::size_t ltarget_size = aserver_map.size() + 1;
			const std::size_t lresv_limit = static_cast<std::size_t>(aserver_map.bucket_count() * aserver_map.max_load_factor());
			if (ltarget_size <= lresv_limit)
			{
				return;
			}

			const std::size_t lnew_cap = std::max<std::size_t>(g_sess_resv, ltarget_size * 2);
			aserver_map.reserve(lnew_cap);
			asession_map.reserve(lnew_cap);
		}

		const tab_servers* find_tab(i32_serverid aserverid)
		{
			return ttab_servers::instance().tab(nnodeid::tid(aserverid));
		}
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
				m_server.reserve(g_sess_resv);
				m_session.reserve(g_sess_resv);
			}
			else
			{
				grow_maps(m_server, m_session);
			}

			if (const auto lserver_it = m_server.find(aserverid); lserver_it != m_server.end())
			{
				// Replace stale mappings so each server id owns at most one live session.
				m_session.erase(lserver_it->second);
				m_server.erase(lserver_it);
			}

			if (const auto lsession_it = m_session.find(asession); lsession_it != m_session.end())
			{
				// Also enforce the reverse invariant: one session belongs to one server id.
				m_server.erase(lsession_it->second);
				m_session.erase(lsession_it);
			}

			m_server.emplace(aserverid, asession);
			m_session.emplace(asession, aserverid);
		}
		if (auto tab = find_tab(aserverid); tab != nullptr)
		{
			log_error()->print("server_session::add [{}:{}_{}]", nnodeid::tid(aserverid), tab->m_name, nnodeid::tcount(aserverid));
		}
	}

	void server_session::remove(i32_sessionid asession)
	{
		i32_serverid lserverid = -1;
		{
			lock_write(m_mutex);
			if (const auto lsession_it = m_session.find(asession); lsession_it != m_session.end())
			{
				// Remove both directions under the same lock to keep the mapping consistent.
				lserverid = lsession_it->second;
				m_server.erase(lserverid);
				m_session.erase(lsession_it);
			}
		}
		if (lserverid != -1)
		{
			if (auto tab = find_tab(lserverid); tab != nullptr)
			{
				log_error()->print("server_session::remove [{}:{}_{}]", nnodeid::tid(lserverid), tab->m_name, nnodeid::tcount(lserverid));
			}
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
			if (const tab_servers* tab = find_tab(aserverid); tab != nullptr)
			{
				asername = tab->m_name;
				return true;
			}
		}
		return false;
	}

	bool server_session::serverinfobysession(i32_sessionid asessionid, std::pair<str_servername, i32_serverid>& apair)
	{
		// Resolve the numeric server id first, then translate it through tab_servers for logging/UI.
		const i32_serverid lserverid = serverid(asessionid);
		if (lserverid == -1)
		{
			return false;
		}
		const tab_servers* ltab = find_tab(lserverid);
		if (ltab == nullptr)
		{
			return false;
		}
		apair.second = lserverid;
		apair.first = ltab->m_name;
		return true;
	}
}//namespace ngl
