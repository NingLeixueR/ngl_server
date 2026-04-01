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
// File overview: Implements logic for kcp.


#include "actor/protocol/nprotocol.h"
#include "net/udp/kcp/kcp_session.h"
#include "actor/actor_base/core/nguid.h"
#include "net/udp/kcp/asio_kcp.h"
#include "tools/log/nlog.h"

#include <unordered_set>
#include <vector>

namespace ngl
{
	int udp_output(const char* buf, int len, ikcpcb*, void* user)
	{
		const auto lpstruct = (kcp_endpoint*)user;
		// KCP emits raw UDP datagrams through this callback.
		lpstruct->m_asiokcp->sendu(lpstruct, buf, len);
		return len;
	}

	kcp_session::kcp_session(asio_kcp* asiokcp) :
		m_sessionid(0),
		m_asiokcp(asiokcp)
	{}

	std::shared_ptr<kcp_endpoint> kcp_session::add_kcp_endpoint(
		int32_t aconv
		, const char* aip
		, i16_port aport
		, i64_actorid aactoridserver
		, i64_actorid aactoridclient
	)
	{
		auto ltemp = std::make_shared<kcp_endpoint>();
		
		ltemp->m_ip = aip;
		ltemp->m_port = aport;
		ltemp->m_asiokcp = m_asiokcp;
		ltemp->m_client = aactoridclient;
		ltemp->m_server = aactoridserver;
		ltemp->create(aconv, m_sessionid, (void*)ltemp.get());
		
		// Configure KCP for low-latency game traffic.
		ltemp->setoutput(udp_output);
		ltemp->nodelay(1, 10, 2, 1);
		ltemp->wndsize(128, 128);
		ltemp->setmtu(512);

		return ltemp;
	}

	std::shared_ptr<kcp_endpoint> kcp_session::erase_session_nolock(i32_sessionid asession)
	{
		std::shared_ptr<kcp_endpoint> lpse = nullptr;
		if (!tools::erasemap(m_dataofsession, asession, lpse) || lpse == nullptr)
		{
			return nullptr;
		}

		if (auto endpoint_map = tools::findmap(m_dataofendpoint, lpse->m_ip); endpoint_map != nullptr)
		{
			endpoint_map->erase(lpse->m_port);
			if (endpoint_map->empty())
			{
				m_dataofendpoint.erase(lpse->m_ip);
			}
		}

		m_actoridofsession.erase(lpse->m_client);
		m_actoridofsession.erase(lpse->m_server);
		// Returning the shared_ptr keeps the endpoint alive until the caller finishes any cleanup.
		return lpse;
	}

	std::shared_ptr<kcp_endpoint> kcp_session::add(int32_t aconv, const basio::ip::udp::endpoint& aendpoint, i64_actorid aactoridserver, i64_actorid aactoridclient)
	{
		if (aconv <= 0)
		{
			return nullptr;
		}
		std::string lip = aendpoint.address().to_string();
		i16_port lport = aendpoint.port();
		int64_t lcreatems = tools::time_wheel::getms();
		auto ltemp = add_kcp_endpoint(aconv, lip.c_str(), lport, aactoridserver, aactoridclient);
		ltemp->m_endpoint = aendpoint;

		{
			lock_write(m_mutex);
			if (auto lsessionid = tools::findmap(m_actoridofsession, aactoridclient); lsessionid != nullptr)
			{
				erase_session_nolock(*lsessionid);
			}
			i32_sessionid lsessionid = ++m_sessionid;
			m_dataofsession.insert_or_assign(lsessionid, ltemp);
			ltemp->m_session = lsessionid;
			m_actoridofsession.insert_or_assign(aactoridclient, lsessionid);
			m_actoridofsession.insert_or_assign(aactoridserver, lsessionid);
			m_dataofendpoint[lip].insert_or_assign(lport, ltemp);
			tools::wheel_parm lparm
			{
				.m_ms = ekcp_update_intervalms,
				.m_intervalms = [](int64_t) {return ekcp_update_intervalms; } ,
				.m_count = 0x7fffffff,
				.m_fun = [this, lsessionid, lcreatems](const tools::wheel_node*)
				{
					std::shared_ptr<kcp_endpoint>* lpse = nullptr;
					{
						lock_write(m_mutex);
						lpse = tools::findmap(m_dataofsession, lsessionid);
					}
					if (lpse != nullptr)
					{
						// KCP's internal timers are driven by the shared wheel rather than a dedicated thread.
						(*lpse)->update((IUINT32)(tools::time_wheel::getms() - lcreatems));
					}
				}
			};
			ltemp->m_timerid = m_kcptimer.addtimer(lparm);
		}
		
		return ltemp;
	}

	std::shared_ptr<kcp_endpoint> kcp_session::reset_add(int32_t aconv, const basio::ip::udp::endpoint& aendpoint, i64_actorid aactoridserver, i64_actorid aactoridclient)
	{
		return add(aconv, aendpoint, aactoridserver, aactoridclient);
	}

	void kcp_session::erasebysession(i32_sessionid asession)
	{
		lock_write(m_mutex);
		erase_session_nolock(asession);
	}

	void kcp_session::erasebyaactorid(i64_actorid aactorid)
	{
		lock_write(m_mutex);
		auto lsessionid = tools::findmap(m_actoridofsession, aactorid);
		if (lsessionid == nullptr)
		{
			return;
		}
		erase_session_nolock(*lsessionid);
	}

	std::shared_ptr<kcp_endpoint> kcp_session::find_info(i32_sessionid asession)
	{
		auto lpse = tools::findmap(m_dataofsession, asession);
		if (lpse == nullptr)
		{
			return nullptr;
		}
		return *lpse;
	}

	std::shared_ptr<kcp_endpoint> kcp_session::find_info(const basio::ip::udp::endpoint& aendpoint)
	{
		std::string lip = aendpoint.address().to_string();
		i16_port lport = aendpoint.port();
	 	auto lpmap = tools::findmap(m_dataofendpoint, lip);
		if (lpmap == nullptr)
		{
			return nullptr;
		}
		auto lpse = tools::findmap(*lpmap, lport);
		if (lpse == nullptr)
		{
			return nullptr;
		}
		return *lpse;
	}

	std::shared_ptr<kcp_endpoint> kcp_session::find(i32_sessionid asession)
	{
		lock_read(m_mutex);
		return find_info(asession);
	}

	std::shared_ptr<kcp_endpoint> kcp_session::findbyactorid(i64_actorid aactorid)
	{
		lock_read(m_mutex);
		auto lpse = tools::findmap(m_actoridofsession, aactorid);
		if (lpse == nullptr)
		{
			return nullptr;
		}
		return find_info(*lpse);
	}

	std::shared_ptr<kcp_endpoint> kcp_session::find(const basio::ip::udp::endpoint& aendpoint)
	{
		lock_read(m_mutex);
		return find_info(aendpoint);
	}

	basio::ip::udp::endpoint* kcp_session::find_endpoint(i32_sessionid asession)
	{
		lock_read(m_mutex);
		std::shared_ptr<kcp_endpoint> lpstruct = find_info(asession);
		if (lpstruct == nullptr)
		{
			return nullptr;
		}
		return &lpstruct->m_endpoint;
	}

	void kcp_session::foreach(const std::function<void(std::shared_ptr<kcp_endpoint>&)>& acall)
	{
		std::vector<std::shared_ptr<kcp_endpoint>> lsessions;
		{
			lock_read(m_mutex);
			// Copy out shared_ptrs first so callbacks can safely mutate session tables.
			lsessions.reserve(m_dataofsession.size());
			for (auto& [_session, _se] : m_dataofsession)
			{
				lsessions.push_back(_se);
			}
		}

		for (auto& session : lsessions)
		{
			acall(session);
		}
	}

	void kcp_session::foreachbyarea(i16_area aarea, const std::function<void(std::shared_ptr<kcp_endpoint>&)>& acall)
	{
		std::vector<std::shared_ptr<kcp_endpoint>> lsessions;
		{
			lock_read(m_mutex);
			lsessions.reserve(m_dataofsession.size());
			std::unordered_set<i32_sessionid> lvisited;
			lvisited.reserve(m_dataofsession.size());
			for (const std::pair<const i64_actorid, i32_sessionid>& lpair : m_actoridofsession)
			{
				if (nguid::area(lpair.first) != aarea)
				{
					continue;
				}

				if (!lvisited.insert(lpair.second).second)
				{
					// One session is indexed by both client and server actor ids; visit it only once.
					continue;
				}

				std::shared_ptr<kcp_endpoint> lptr = find_info(lpair.second);
				if (lptr != nullptr)
				{
					lsessions.push_back(lptr);
				}
			}
		}

		for (auto& session : lsessions)
		{
			acall(session);
		}
	}
}//namespace ngl
