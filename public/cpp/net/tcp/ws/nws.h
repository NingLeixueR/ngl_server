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
// File overview: Declares interfaces for websocket logic.

#pragma once

#include "tools/serialize/netbuff_pool.h"
#include "actor/actor_base/handle_pram.h"
#include "tools/serialize/structbytes.h"
#include "actor/actor_base/actor_base.h"
#include "tools/serialize/segpack.h"
#include "tools/serialize/pack.h"
#include "net/server_session.h"
#include "net/tcp/ws/asio_ws.h"
#include "net/net_pack.h"

#include <list>
#include <map>
#include <memory>
#include <set>
#include <shared_mutex>
#include <vector>

namespace ngl
{
	struct nws
	{
	private:
		std::shared_ptr<asio_ws>				m_server = nullptr;			// WebSocket transport implementation.
		std::vector<std::shared_ptr<segpack>>	m_segpackvec;				// One packet reassembler per socket worker thread.
		i16_port								m_port = 0;					// Bound listen port.
		i32_threadsize							m_socketthreadnum = 0;		// Number of socket worker threads.
		bool									m_outernet = false;			// Whether public-network clients are allowed.
		bpool									m_pool;						// Shared pack allocation pool for outbound traffic.
		std::shared_mutex						m_mutex;					// Reserved for higher-level coordination.
		std::list<pack>							m_packlist;					// Reserved pack queue.
	private:
		// WebSocket text frames are not mapped into actor protocol dispatch here;
		// binary frames still reuse the existing pack/segpack pipeline.
		bool socket_recv(service_ws* ap, const char* abuff, int32_t abufflen);
	public:
		// Singleton WebSocket service used by higher-level network wrappers.
		static nws& instance()
		{
			static nws ltemp;
			return ltemp;
		}

		// Access the shared outbound pack pool.
		bpool& pool();

		// Start the WebSocket server/client service.
		bool init(i16_port aport, i32_threadsize asocketthreadnum, bool aouternet);

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

		// Connect to one websocket endpoint, with optional wait/reconnect behavior.
		bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);
		bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun, bool await, bool areconnection);
		bool connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection);

		// Send raw text, typed packs, or typed payloads over websocket.
		bool send_msg(i32_sessionid asession, const std::string& amsg);
		bool send_pack(i32_sessionid asession, std::shared_ptr<pack>& lpack);
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
}// namespace ngl


namespace ngl
{
	template <typename Y, typename T/* = Y*/>
	bool nws::send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
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
	bool nws::send(const std::map<i32_sessionid, i64_actorid>& asession, const Y& adata, i64_actorid aactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&pool(), adata, aactorid, 0);
		if (lpack == nullptr)
		{
			return false;
		}
		return send(asession, aactorid, lpack);
	}

	template <typename Y, typename T/* = Y*/>
	bool nws::send(const std::set<i32_sessionid>& asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&pool(), adata, aactorid, 0);
		if (lpack == nullptr)
		{
			return false;
		}
		return send(asession, aactorid, arequestactorid, lpack);
	}

	// Resolve one server id to its live session before sending.
	template <typename Y, typename T/* = Y*/>
	bool nws::send_server(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		i32_sessionid lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return send<Y, T>(lsession, adata, aactorid, arequestactorid);
	}

	template <typename Y, typename T/* = Y*/>
	bool nws::send_server(const std::set<i32_serverid>& aserverids, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		std::set<i32_session> lsessions;
		for (i32_serverid iserverid : aserverids)
		{
			i32_session lsession = server_session::sessionid(iserverid);
			if (lsession != -1)
			{
				lsessions.insert(lsession);
			}
		}
		if (!lsessions.empty())
		{
			return send<Y, T>(lsessions, adata, aactorid, arequestactorid);
		}
		return false;
	}

	template <typename T>
	bool nws::send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata)
	{
		std::vector<std::pair<i32_actordataid, i16_area>> lvecs = { {auid, aarea} };
		return send_client(lvecs, agateway, adata);
	}

	// Wrap one payload into a gateway-forward packet and fan it out to the
	// gateway sessions responsible for the target users.
	template <typename T>
	bool nws::send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata)
	{
		np_actor_forward<T, forward_g2c<forward>> pro;
		for (std::size_t i = 0; i < avec.size(); ++i)
		{
			pro.m_data.m_uid.push_back(avec[i].first);
			pro.m_data.m_area.push_back(avec[i].second);
		}

		forward& lforward = pro.m_data.m_data;

		ngl::ser::serialize_byte lserializebyte;
		ngl::ser::nserialize::bytes(&lserializebyte, adata);

		lforward.m_bufflen = lserializebyte.pos();
		lforward.m_buff = netbuff_pool::instance().malloc_private(lforward.m_bufflen);
		if (lforward.m_buff == nullptr && lforward.m_bufflen > 0)
		{
			return false;
		}

		ngl::ser::serialize_push lserializepush(const_cast<char*>(lforward.m_buff), lforward.m_bufflen);
		if (ngl::ser::nserialize::push(&lserializepush, adata))
		{
			if (agateway != 0)
			{
				i32_session lsession = server_session::sessionid(agateway);
				if (lsession > 0)
				{
					const bool lsent = send(lsession, pro, nguid::make(), nguid::make());
					netbuff_pool::instance().free((char*)lforward.m_buff);
					return lsent;
				}
			}
		}
		netbuff_pool::instance().free((char*)lforward.m_buff);
		return false;
	}
}
