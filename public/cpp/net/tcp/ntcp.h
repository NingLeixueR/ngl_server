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
// File overview: Declares interfaces for tcp.


#pragma once

#include "actor/actor_base/core/handle_pram.h"
#include "tools/serialize/structbytes.h"
#include "tools/serialize/socket_pool.h"
#include "tools/serialize/segpack.h"
#include "tools/serialize/pack.h"
#include "net/server_session.h"
#include "net/tcp/asio_tcp.h"
#include "net/net_pack.h"

#include <vector>
#include <memory>

namespace ngl
{
	class ntcp
	{
		std::shared_ptr<asio_tcp>				m_server = nullptr;			// TCP transport implementation.
		std::vector<std::shared_ptr<segpack>>	m_segpackvec;				// One frame reassembler per socket worker thread.
		i16_port								m_port = 0;					// Bound listen port.
		bool									m_outernet = false;			// Whether public-network clients are allowed.
		bpool									m_pool;						// Shared pack allocation pool for outbound traffic.
	private:
		// Pass raw socket bytes through LAN checks and the per-thread frame reassembler.
		bool socket_recv(service_io* ap, const char* abuff, int32_t abufflen);
	public:
		// Singleton TCP service used by the actor/network layers.
		static ntcp& instance()
		{
			static ntcp ltemp;
			return ltemp;
		}

		// Access the shared outbound pack pool.
		bpool& pool();

		// Start the TCP server/client service.
		bool init(i16_port aport, i32_threadsize asocketthreadnum, bool	aouternet);

		// Return the bound listen port.
		i16_port port();

		// Close a session and notify actor-level connection handlers.
		void close(i32_sessionid asession);

		// Close a session without actor-level notifications.
		void close_net(i32_sessionid asession);

		// Register automatic reconnect behavior for a session.
		void set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);

		// Pick public IP for robot nodes, private IP otherwise.
		const std::string& ip(const net_works& anets);

		// Start an asynchronous connect request. Returns true only if the transport
		// accepted the request for execution.
		bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);

		// Same as above, with optional blocking wait and reconnect registration.
		// The return value still reports whether the async connect was successfully
		// started, not whether the remote side eventually accepted it.
		bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun, bool await, bool areconnection);

		// Resolve one configured server id and connect to its advertised address.
		bool connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection);
		
		// Send raw text, typed packs, or typed payloads over TCP.
		bool send_msg(i32_sessionid asession, const std::string& amsg);

		// Send one already-built typed packet.
		bool send_pack(i32_sessionid asession, std::shared_ptr<pack>& lpack);

		// Type-erased overload used by generic forwarding paths.
		bool send_pack(i32_sessionid asession, std::shared_ptr<void>& lpack);

		// Convenience wrapper that resolves a server id to its session.
		bool send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		// Serialize and send one typed payload to a single session.
		template <typename Y, typename T = Y>
		bool send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		template <typename Y, typename T = Y>
		bool send(const std::map<i32_sessionid, i64_actorid>& asession, const Y& adata, i64_actorid aactorid);

		template <typename Y, typename T = Y>
		bool send(const std::set<i32_sessionid>& asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// Resolve the server session first, then serialize and send the payload.
		template <typename Y, typename T = Y>
		bool send_server(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		template <typename Y, typename T = Y>
		bool send_server(const std::set<i32_serverid>& aserverids, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// Broadcast an already-built pack to many sessions.
		bool send(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::shared_ptr<pack>& apack);
		bool send(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack);

		// Send a forward_g2c wrapper toward one gateway or one gateway/user batch.
		template <typename T>
		bool send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata);

		template <typename T>
		bool send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata);
	};
}//namespace ngl


namespace ngl
{
	template <typename Y, typename T/* = Y*/>
	bool ntcp::send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid, false);
		if (lpack == nullptr)
		{
			return false;
		}
		if (send_pack(asession, lpack) == false)
		{
			return false;
		}
		return true;
	}

	template <typename Y, typename T/* = Y*/>
	bool ntcp::send(const std::map<i32_sessionid, i64_actorid>& asession, const Y& adata, i64_actorid aactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&pool(), adata, aactorid, nguid::make(), false);
		if (lpack == nullptr)
		{
			return false;
		}
		return send(asession, aactorid, lpack);
	}

	template <typename Y, typename T/* = Y*/>
	bool ntcp::send(const std::set<i32_sessionid>& asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&pool(), adata, aactorid, arequestactorid, false);
		if (lpack == nullptr)
		{
			return false;
		}
		return send(asession, aactorid, arequestactorid, lpack);
	}

	// Resolve one server id to its live session before sending.
	template <typename Y, typename T/* = Y*/>
	bool ntcp::send_server(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		i32_sessionid lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return send<Y, T>(lsession, adata, aactorid, arequestactorid);
	}

	template <typename Y, typename T/* = Y*/>
	bool ntcp::send_server(const std::set<i32_serverid>& aserverids, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		if (aserverids.empty())
		{
			return false;
		}
		bool lfound = false;
		bool lret = true;
		std::shared_ptr<pack> lpack = nullptr;
		for (i32_serverid iserverid : aserverids)
		{
			i32_session lsession = server_session::sessionid(iserverid);
			if (lsession > 0)
			{
				if (lpack == nullptr)
				{
					lpack = net_pack<T>::npack(&pool(), adata, aactorid, arequestactorid, false);
					if (lpack == nullptr)
					{
						return false;
					}
				}
				lfound = true;
				if (!send_pack(lsession, lpack))
				{
					lret = false;
				}
			}
		}
		if (!lfound)
		{
			return false;
		}
		return lret;
	}

	template <typename T>
	bool ntcp::send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata)
	{
		std::vector<std::pair<i32_actordataid, i16_area>> lvecs = { {auid, aarea} };
		return send_client(lvecs, agateway, adata);
	}

	// Wrap one payload into a gateway-forward packet and fan it out to the
	// gateway sessions responsible for the target users.
	template <typename T>
	bool ntcp::send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata)
	{
		if (agateway == 0)
		{
			return false;
		}
		i32_session lsession = server_session::sessionid(agateway);
		if (lsession <= 0)
		{
			return false;
		}

		np_actor_forward<T, forward_g2c<forward>> pro;
		pro.m_data.m_uid.reserve(avec.size());
		pro.m_data.m_area.reserve(avec.size());
		for (const auto& [luid, larea] : avec)
		{
			pro.m_data.m_uid.emplace_back(luid);
			pro.m_data.m_area.emplace_back(larea);
		}

		forward& lforward = pro.m_data.m_data;

		ngl::ser::serialize_byte lserializebyte;
		ngl::ser::nserialize::bytes(&lserializebyte, adata);

		lforward.m_bufflen = lserializebyte.pos();
		char* lbuf = socket_pool::malloc(lforward.m_bufflen);
		lforward.m_buff = lbuf;
		if (lbuf == nullptr && lforward.m_bufflen > 0)
		{
			return false;
		}

		ngl::ser::serialize_push lserializepush(lbuf, lforward.m_bufflen);
		if (ngl::ser::nserialize::push(&lserializepush, adata))
		{
			const bool lsent = send(lsession, pro, nguid::make(), nguid::make());
			socket_pool::free(lbuf);
			return lsent;
		}
		socket_pool::free(lbuf);
		return false;
	}
}

