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
// File overview: Declares session-based transport dispatch across tcp and ws.

#pragma once

#include "net/net_session.h"
#include "net/tcp/ntcp.h"
#include "net/tcp/ws/nws.h"

#include <map>
#include <memory>
#include <set>
#include <string>

namespace ngl
{
	class nnet
	{
		nnet() = default;
	public:
		static nnet& instance()
		{
			static nnet ltemp;
			return ltemp;
		}

		ENET_PROTOCOL protocol(i32_sessionid asession) const;

		// Use the TCP pool as the default generic pack allocator.
		bpool& pool();
		bpool& pool(ENET_PROTOCOL aprotocol);
		bpool& pool(i32_sessionid asession);

		void close(i32_sessionid asession);
		void close_net(i32_sessionid asession);

		bool send_msg(i32_sessionid asession, const std::string& amsg);
		bool send_pack(i32_sessionid asession, std::shared_ptr<pack>& apack);
		bool send_pack(i32_sessionid asession, std::shared_ptr<void>& apack);
		bool send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);
		bool send_server(i32_serverid aserverid, std::shared_ptr<void>& apack);

		bool send(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::shared_ptr<pack>& apack);
		bool send(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack);

		template <typename Y, typename T = Y>
		bool send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			switch (protocol(asession))
			{
			case ENET_TCP:
				return ntcp::instance().send<Y, T>(asession, adata, aactorid, arequestactorid);
			case ENET_WS:
				return nws::instance().send<Y, T>(asession, adata, aactorid, arequestactorid);
			default:
				return false;
			}
		}

		template <typename Y, typename T = Y>
		bool send(const std::map<i32_sessionid, i64_actorid>& asession, const Y& adata, i64_actorid aactorid)
		{
			std::map<i32_sessionid, i64_actorid> ltcpmap;
			std::map<i32_sessionid, i64_actorid> lwsmap;
			split_sessions(asession, ltcpmap, lwsmap);

			bool lret = true;
			if (!ltcpmap.empty() && !ntcp::instance().send<Y, T>(ltcpmap, adata, aactorid))
			{
				lret = false;
			}
			if (!lwsmap.empty() && !nws::instance().send<Y, T>(lwsmap, adata, aactorid))
			{
				lret = false;
			}
			return lret;
		}

		template <typename Y, typename T = Y>
		bool send(const std::set<i32_sessionid>& asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::set<i32_sessionid> ltcpset;
			std::set<i32_sessionid> lwsset;
			split_sessions(asession, ltcpset, lwsset);

			bool lret = true;
			if (!ltcpset.empty() && !ntcp::instance().send<Y, T>(ltcpset, adata, aactorid, arequestactorid))
			{
				lret = false;
			}
			if (!lwsset.empty() && !nws::instance().send<Y, T>(lwsset, adata, aactorid, arequestactorid))
			{
				lret = false;
			}
			return lret;
		}

		template <typename Y, typename T = Y>
		bool send_server(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			i32_sessionid lsession = server_session::sessionid(aserverid);
			if (lsession == -1)
			{
				return false;
			}
			return send<Y, T>(lsession, adata, aactorid, arequestactorid);
		}

		template <typename Y, typename T = Y>
		bool send_server(const std::set<i32_serverid>& aserverids, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::set<i32_sessionid> lsessions;
			for (i32_serverid lserverid : aserverids)
			{
				i32_sessionid lsession = server_session::sessionid(lserverid);
				if (lsession > 0)
				{
					lsessions.insert(lsession);
				}
			}
			if (lsessions.empty())
			{
				return false;
			}
			return send<Y, T>(lsessions, adata, aactorid, arequestactorid);
		}
	private:
		static void split_sessions(
			const std::map<i32_sessionid, i64_actorid>& asession,
			std::map<i32_sessionid, i64_actorid>& atcpmap,
			std::map<i32_sessionid, i64_actorid>& awsmap
		)
		{
			for (const auto& [lsessionid, lactorid] : asession)
			{
				switch (net_session::protocol(lsessionid))
				{
				case ENET_TCP:
					atcpmap.insert(std::make_pair(lsessionid, lactorid));
					break;
				case ENET_WS:
					awsmap.insert(std::make_pair(lsessionid, lactorid));
					break;
				default:
					break;
				}
			}
		}

		static void split_sessions(
			const std::set<i32_sessionid>& asession,
			std::set<i32_sessionid>& atcpset,
			std::set<i32_sessionid>& awsset
		)
		{
			for (i32_sessionid lsessionid : asession)
			{
				switch (net_session::protocol(lsessionid))
				{
				case ENET_TCP:
					atcpset.insert(lsessionid);
					break;
				case ENET_WS:
					awsset.insert(lsessionid);
					break;
				default:
					break;
				}
			}
		}
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool handle_send<T>::send_server(i32_serverid aserverid, const handle_pram& adata)
	{
		return nnet::instance().send_server<T>(aserverid, *(T*)adata.m_data.get(), adata.m_actor, adata.m_requestactor);
	}

	template <typename T>
	bool handle_send<T>::send_server(i32_serverid aserverid, const nguid& aactorid, const nguid& arequestactorid, const T& adata)
	{
		return nnet::instance().send_server<T>(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nnet::instance().send_server<T>(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send_server(const std::set<i32_serverid>& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nnet::instance().send_server<T>(aserverids, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool handle_pram::send(i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid)
	{
		return nnet::instance().send(asession, adata, aactorid.id(), arequestactorid.id());
	}

	template <typename T>
	std::shared_ptr<pack> actor_base::net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ::ngl::net_pack<T>::npack(&nnet::instance().pool(), adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nnet::instance().send(asession, adata, aactorid, arequestactorid);
	}
}//namespace ngl
