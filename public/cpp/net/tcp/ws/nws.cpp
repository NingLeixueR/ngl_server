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
// File overview: Implements logic for websocket.


#include "actor/actor_logic/actor_gateway/actor_gateway.h"
#include "actor/actor_base/core/nguid.h"
#include "actor/tab/ttab_servers.h"
#include "actor/protocol/protocol.h"
#include "net/tcp/ws/nws.h"

namespace ngl
{
	bool nws::socket_recv(service_ws* ap, const char* abuff, int32_t abufflen)
	{
		if (ap == nullptr)
		{
			return false;
		}
		if (abuff == nullptr && abufflen == 0)
		{
			// Upstream transport uses a null/zero callback to signal closure.
			close(ap->m_sessionid);
			return true;
		}
		if (m_outernet == false)
		{
			if (ap->m_is_lanip == false)
			{
				// Internal-only deployments reject public-network peers at the socket layer.
				return false;
			}
		}
		// segpack preserves the existing pack protocol on top of websocket binary frames.
		return m_segpackvec[ap->m_threadid]->push(ap->m_sessionid, abuff, abufflen, ap->m_is_lanip);
	}

	bpool& nws::pool()
	{
		return m_pool;
	}

	bool nws::init(i16_port aport, i32_threadsize asocketthreadnum, bool aouternet, bool ause_tls, const ws_tls_options& atls_options)
	{
		if (m_server != nullptr)
		{
			return true;
		}
		if (asocketthreadnum > net_config_socket_pthread_max_size || asocketthreadnum <= 0)
		{
			asocketthreadnum = net_config_socket_pthread_max_size;
		}

		m_outernet = aouternet;
		m_port = aport;

		m_segpackvec.clear();
		m_segpackvec.reserve(static_cast<std::size_t>(asocketthreadnum));
		for (int i = 0; i < asocketthreadnum; ++i)
		{
			// Each socket worker keeps its own packet reassembler state.
			m_segpackvec.push_back(std::make_shared<segpack>());
		}

		std::function<bool(service_ws*, const char*, uint32_t)> lrecv = std::bind_front(&nws::socket_recv, this);

		std::function<void(i32_sessionid)> lclose = [this](i32_sessionid asession)
			{
				// Losing the DB connection is fatal because gameplay state can no longer persist safely.
				i32_serverid lserverid = server_session::serverid(asession);
				if (lserverid != -1 && ttab_servers::instance().node_type(nnodeid::tid(lserverid)) == NODE_TYPE::DB)
				{
					tools::no_core_dump();
				}
				close(asession);
			};

		std::function<void(i32_sessionid, bool, const pack*)> lsendfinish = [](i32_sessionid asessionid, bool abool, const pack*)
			{
				if (abool)
				{
					std::pair<str_servername, i32_serverid> lpair("none", -1);
					server_session::serverinfobysession(asessionid, lpair);
					log_error()->print("ws send finish fail sessionid:{} server[{}:{}]", asessionid, lpair.second, lpair.first);
				}
			};

		if (port() > 0)
		{
			m_server = std::make_shared<asio_ws>(port(), asocketthreadnum, ause_tls, lrecv, lclose, lsendfinish, atls_options);
		}
		else
		{
			m_server = std::make_shared<asio_ws>(asocketthreadnum, ause_tls, lrecv, lclose, lsendfinish, atls_options);
		}
		return true;
	}

	i16_port nws::port()
	{
		return m_port;
	}

	void nws::close(i32_sessionid asession)
	{
		std::pair<str_servername, i32_serverid> lpair("none", -1);
		if (server_session::serverinfobysession(asession, lpair))
		{
			log_error()->print("ws close sessionid:{} server[{}:{}]", asession, lpair.second, lpair.first);
		}
		if (m_server != nullptr)
		{
			m_server->close_net(asession);
		}

		// Notify gateway-side actors so they can clean up player/session state.
		auto pro = std::make_shared<np_actor_session_close>();
		pro->m_sessionid = asession;
		i64_actorid lactorid = actor_gateway::actorid(nconfig.tcount());
		actor::send_actor(lactorid, nguid::make(), pro);

		server_session::remove(asession);
	}

	void nws::close_net(i32_sessionid asession)
	{
		if (m_server == nullptr)
		{
			return;
		}
		m_server->close_net(asession);
	}

	void nws::set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun)
	{
		if (m_server == nullptr)
		{
			return;
		}
		m_server->set_close(asession, [this, aip, aport, afun]()
			{
				connect(aip, aport, afun);
			}
		);
	}

	bool nws::connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun)
	{
		if (m_server == nullptr)
		{
			return false;
		}
		// Return true only when the transport accepted the async connect request.
		return m_server->connect(aip, aport, afun) != nullptr;
	}

	bool nws::connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun, bool await, bool areconnection /* Whether */)
	{
		std::shared_ptr<ngl::tools::sem> lsem = nullptr;
		if (await)
		{
			// Optional blocking wait is implemented with a small semaphore wrapper.
			lsem = std::make_shared<ngl::tools::sem>();
		}
		if (!connect(aip, aport, [this, afun, aip, aport, areconnection, lsem](i32_sessionid asession)
			{
				if (afun != nullptr)
				{
					afun(asession);
				}
				if (lsem != nullptr)
				{
					lsem->post();
				}
				if (areconnection && asession > 0)
				{
					// Re-register the reconnect hook only after a successful connect.
					set_close(asession, aip, aport, afun);
				}
			}
		))
		{
			return false;
		}
		if (lsem != nullptr)
		{
			lsem->wait();
		}
		return true;
	}

	const std::string& nws::ip(const net_works& anets)
	{
		return nconfig.nodetype() == ROBOT ? anets.m_ip : anets.m_nip;
	}

	bool nws::connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession != -1)
		{
			if (afun != nullptr)
			{
				afun(lsession);
			}
			return true;
		}

		net_works lnets;
		const tab_servers* ltab = ttab_servers::instance().tab(nnodeid::tid(aserverid));
		if (ltab == nullptr || !ttab_servers::instance().get_nworks(ltab, ENET_WS, nnodeid::tcount(aserverid), lnets))
		{
			return false;
		}

		const std::string& lip = ip(lnets);
		const i16_port lport = lnets.m_port;

		log_info()->print("Connect WS Server {}@{}:{}", aserverid, lip, lport);
		return connect(lip, lport, [aserverid, afun](i32_session asession)
			{
				if (asession > 0)
				{
					// server_session is the canonical server id <-> websocket session map.
					server_session::add(aserverid, asession);
				}
				if (afun != nullptr)
				{
					afun(asession);
				}
			}, await, areconnection
		);
	}

	bool nws::send_msg(i32_sessionid asession, const std::string& amsg)
	{
		const int lcount = static_cast<int>(amsg.size());
		auto lpack = pack::make_pack(&m_pool, lcount + 1);
		if (lpack == nullptr || lpack->m_buff == nullptr)
		{
			return false;
		}
		lpack->m_head = nullptr;
		memcpy(lpack->m_buff, amsg.c_str(), lcount);
		lpack->m_buff[lcount] = '\0';
		lpack->m_len = lcount + 1;
		lpack->m_pos = lcount + 1;
		// Telnet/admin traffic uses a raw text buffer instead of the binary pack header format.
		return send_pack(asession, lpack);
	}

	bool nws::send_pack(i32_sessionid asession, std::shared_ptr<pack>& lpack)
	{
		if (m_server == nullptr || lpack == nullptr)
		{
			return false;
		}
		return m_server->send(asession, lpack);
	}

	bool nws::send_pack(i32_sessionid asession, std::shared_ptr<void>& lpack)
	{
		if (m_server == nullptr || lpack == nullptr)
		{
			return false;
		}
		return m_server->send(asession, lpack);
	}

	bool nws::send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_sessionid lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return send_pack(lsession, apack);
	}

	bool nws::send(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::shared_ptr<pack>& apack)
	{
		if (apack == nullptr || asession.empty())
		{
			return false;
		}
		bool lret = true;
		for (const auto& [lsession, lactorid] : asession)
		{
			apack->set_actor(lactorid, aactorid);
			if (!send_pack(lsession, apack))
			{
				lret = false;
			}
		}
		return lret;
	}

	bool nws::send(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack)
	{
		if (apack == nullptr || asession.empty())
		{
			return false;
		}
		bool lret = true;
		apack->set_actor(aactorid, arequestactorid);
		for (i32_sessionid lsession : asession)
		{
			if (!send_pack(lsession, apack))
			{
				lret = false;
			}
		}
		return lret;
	}
}// namespace ngl
