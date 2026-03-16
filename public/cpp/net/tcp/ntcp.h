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

#include "tools/serialize/netbuff_pool.h"
#include "actor/actor_base/handle_pram.h"
#include "tools/serialize/structbytes.h"
#include "actor/actor_base/actor_base.h"
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
		std::shared_ptr<asio_tcp>				m_server = nullptr;			// asio
		std::vector<std::shared_ptr<segpack>>	m_segpackvec;				// Pack
		i16_port								m_port = 0;					// Server port
		i32_threadsize							m_socketthreadnum = 0;		// Socket datathread
		bool									m_outernet = false;			// Whether connection
		bpool									m_pool;						// Send
		std::shared_mutex						m_mutex;					// Translated comment.
		std::list<pack>							m_packlist;					// Sendqueue
	private:
		bool socket_recv(service_io* ap, const char* abuff, int32_t abufflen);
	public:
		// # And singleton, canaddinstance1....n
		// # In port
		static ntcp& instance()
		{
			static ntcp ltemp;
			return ltemp;
		}

		bpool& pool();

		// # Port thread
		bool init(i16_port aport, i32_threadsize asocketthreadnum, bool	aouternet);

		// # Port
		i16_port port();

		// # Closesocketconnectionandload data
		// # Notifyon
		void close(i32_sessionid asession);

		// # Closesession
		void close_net(i32_sessionid asession);

		// # Setsocketcloseafter,afun:connect parm3
		void set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);

		// # Getserverip
		const std::string& ip(const net_works& anets);

		// # Connectionip:aport
		bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);

		// # Connectionspecifiedip/port
		// # Aip/aport connection ip/port
		// # Afun connectioncallback
		// # Await whether connectionsuccessful
		// # Areconnection whether
		bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun, bool await, bool areconnection);

		// # Connectionspecifiedserver
		bool connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection);
		
		// # Sendmessage
		bool send_msg(i32_sessionid asession, const std::string& amsg);

		// # Sendmessage
		bool send_pack(i32_sessionid asession, std::shared_ptr<pack>& lpack);

		// # Sendmessage
		bool send_pack(i32_sessionid asession, std::shared_ptr<void>& lpack);

		// # To serversendpack
		bool send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		// # Sendmessage
		template <typename Y, typename T = Y>
		bool send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		template <typename Y, typename T = Y>
		bool send(const std::map<i32_sessionid, i64_actorid>& asession, const Y& adata, i64_actorid aactorid);

		template <typename Y, typename T = Y>
		bool send(const std::set<i32_sessionid>& asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// # Toserversendmessage
		template <typename Y, typename T = Y>
		bool send_server(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		template <typename Y, typename T = Y>
		bool send_server(const std::set<i32_serverid>& aserverids, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// # Toa group ofsesionsendmessage
		bool send(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::shared_ptr<pack>& apack);
		bool send(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack);

		// # Toclientsendmessage
		template <typename T>
		bool send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata);

		// # Toclientsendmessage
		template <typename T>
		bool send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata);
	};
}//namespace ngl


namespace ngl
{
	template <typename Y, typename T/* = Y*/>
	bool ntcp::send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
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
	bool ntcp::send(const std::map<i32_sessionid, i64_actorid>& asession, const Y& adata, i64_actorid aactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&pool(), adata, aactorid, 0);
		if (lpack == nullptr)
		{
			return false;
		}
		return send(asession, aactorid, lpack);
	}

	template <typename Y, typename T/* = Y*/>
	bool ntcp::send(const std::set<i32_sessionid>& asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&pool(), adata, aactorid, 0);
		if (lpack == nullptr)
		{
			return false;
		}
		return send(asession, aactorid, arequestactorid, lpack);
	}

	// # Toserversendmessage
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
	bool ntcp::send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata)
	{
		std::vector<std::pair<i32_actordataid, i16_area>> lvecs = { {auid, aarea} };
		return send_client(lvecs, agateway, adata);
	}

	// # Toclientsendmessage
	template <typename T>
	bool ntcp::send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata)
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

namespace ngl
{
	template <typename T>
	bool handle_send<T>::send_server(i32_serverid aserverid, const handle_pram& adata)
	{
		return ntcp::instance().send_server<T>(aserverid, *(T*)adata.m_data.get(), adata.m_actor, adata.m_requestactor);
	}

	template <typename T>
	bool handle_send<T>::send_server(i32_serverid aserverid, const nguid& aactorid, const nguid& arequestactorid, const T& adata)
	{
		return ntcp::instance().send_server<T>(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ntcp::instance().send_server<T>(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send_server(const std::set<i32_serverid>& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ntcp::instance().send_server<T>(aserverids, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool handle_pram::send(i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid)
	{
		return ntcp::instance().send(asession, adata, aactorid.id(), arequestactorid.id());
	}

	template <typename T>
	std::shared_ptr<pack> actor_base::net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ::ngl::net_pack<T>::npack(&ntcp::instance().pool(), adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ntcp::instance().send(asession, adata, aactorid, arequestactorid);
	}
}//namespace ngl