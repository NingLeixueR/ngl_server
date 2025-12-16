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
#pragma once

#include "ttab_servers.h"
#include "net_protocol.h"

namespace ngl
{
	class nets
	{
		nets() = delete;
		nets(const nets&) = delete;
		nets& operator=(const nets&) = delete;

		static std::array<net_protocol*, ENET_COUNT>	m_net;
		static std::map<int16_t, ukcp*>					m_kcpnet;
		static int16_t									m_kcpindex;
	public:
		static net_protocol* net_first();

		static ENET_PROTOCOL session2type(i32_session asession);

		static net_protocol* net(i32_session asession);

		static net_protocol* nettype(ENET_PROTOCOL atype);

		static bool init(i32_threadsize asocketthreadnum, bool aouternet, const std::set<pbnet::ENUM_KCP>& akcp);

		// 服务器只监听一个端口
		static bool check_serverkcp();

		// robot 创建随机端口
		static i16_port create_kcp();

		// server 创建指定端口
		static i16_port create_kcp(pbnet::ENUM_KCP aenum);

		static i16_port kcp_port(int32_t atid, int16_t atcount, pbnet::ENUM_KCP aenum);

		// 获取实例 
		static ukcp* kcp(i16_port auport);
		static ukcp* serkcp(pbnet::ENUM_KCP aenum, int16_t atcount);

		template <typename Y, typename T = Y>
		static bool sendbyserver(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			i32_session lsession = server_session::sessionid(aserverid);
			if (lsession == -1)
			{
				return false;
			}
			return sendbysession<Y,T>(lsession, adata, aactorid, arequestactorid);
		}

		template <typename Y, typename T = Y>
		static bool sendbyserver(const std::set<i32_serverid>& aserverids, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::set<i32_session> lsessionvec;
			for (i32_serverid iserverid : aserverids)
			{
				i32_session lsession = server_session::sessionid(iserverid);
				if (lsession != -1)
				{
					lsessionvec.insert(lsession);
				}
			}
			if (!lsessionvec.empty())
			{
				return sendmore<Y, T>(lsessionvec, adata, aactorid, arequestactorid);
			}
			return false;
		}

		template <typename Y, typename T = Y>
		static bool sendbysession(i32_session asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			net_protocol* lpprotocol = net(asession);
			if (lpprotocol == nullptr)
			{
				return false;
			}
			return lpprotocol->send<Y,T>(asession, adata, aactorid, arequestactorid);
		}

		template <typename Y, typename T = Y>
		static bool sendmore(const std::map<i32_sessionid, i64_actorid>& asession, const Y& adata, i64_actorid aactorid)
		{
			std::map<ENET_PROTOCOL, std::map<i32_sessionid, i64_actorid>> lmap;
			for (const auto& lpair : asession)
			{
				ENET_PROTOCOL ltype = session2type(lpair.first);
				if (ttab_servers::instance().isefficient(ltype))
				{
					lmap[ltype].insert(lpair);
				}
			}
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&nets::net_first()->get_pool(), adata, aactorid, 0);
			if (lpack == nullptr)
			{
				return false;
			}
			for (const auto& lpair : lmap)
			{
				net_protocol* lpprotocol = ngl::nets::nettype(lpair.first);
				if (lpprotocol != nullptr)
				{
					lpprotocol->sendmore(lpair.second, aactorid, lpack);
				}
			}
			return true;
		}

		template <typename Y, typename T = Y>
		static bool sendmore(const std::set<i32_sessionid>& asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::map<ENET_PROTOCOL, std::set<i32_sessionid>> lmap;
			for (i32_sessionid asession : asession)
			{
				ENET_PROTOCOL ltype = session2type(asession);
				if (ttab_servers::instance().isefficient(ltype))
				{
					lmap[ltype].insert(asession);
				}
			}
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&nets::net_first()->get_pool(), adata, aactorid, 0);
			if (lpack == nullptr)
			{
				return false;
			}
			for (const auto& apair : lmap)
			{
				net_protocol* lpprotocol = ngl::nets::nettype(apair.first);
				if (lpprotocol != nullptr)
				{
					lpprotocol->sendmore(apair.second, aactorid, arequestactorid, lpack);
				}
			}
			return true;
		}

		static bool sendpack(i32_sessionid asession, std::shared_ptr<pack>& apack);

		static bool sendpack(i32_sessionid asession, std::shared_ptr<void>& apack);

		static bool sendmsg(i32_sessionid asession, const std::string& amsg);

		static bool ipport(i32_serverid aserverid, std::tuple<ENET_PROTOCOL, str_ip, i16_port>& apair);

		static bool connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection);
	};
}

namespace ngl
{
	template <typename T>
	bool handle_pram_send<T>::sendbyserver(i32_serverid aserverid, const handle_pram& adata)
	{
		return nets::sendbyserver(aserverid, *(T*)adata.m_data.get(), adata.m_actor, adata.m_requestactor);
	}

	template <typename T>
	bool handle_pram_send<T>::sendbyserver(i32_serverid aserverid, const nguid& aactorid, const nguid& arequestactorid, const T& adata)
	{
		return nets::sendbyserver(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool handle_pram::netsend(i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid)
	{
		return nets::sendbysession(asession, adata, aactorid.id(), arequestactorid.id());
	}

	template <typename T>
	bool actor_base::send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nets::sendbyserver(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send_server(const std::set<i32_serverid>& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nets::sendbyserver(aserverids, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::sendpack_server(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return nets::sendpack(lsession, apack);
	}

	template <typename T>
	bool actor_base::sendpack_session(i32_sessionid asession, std::shared_ptr<pack>& apack)
	{
		return nets::sendpack(asession, apack);
	}

	template <typename T>
	std::shared_ptr<pack> actor_base::net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ::ngl::net_pack<T>::npack(&nets::net_first()->get_pool(), adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nets::sendbysession(asession, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send_kcp(i64_actorid aactorid, T& adata, i16_port auport/* = 0*/)
	{
		return send_kcp({ aactorid }, adata, auport);
	}

	template <typename T>
	bool actor_base::send_kcp(const std::set<i64_actorid>& aactorids, T& adata, i16_port auport/* = 0*/)
	{
		ukcp* lpukcp = nets::kcp(auport);
		if (lpukcp == nullptr)
		{
			return false;
		}
		return lpukcp->send(aactorids, adata);
	}
}//namespace ngl