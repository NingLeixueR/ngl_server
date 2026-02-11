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

#include "server_session.h"
#include "tprotocol.h"
#include "nprotocol.h"
#include "nguid.h"
#include "type.h"

#include <cstdint>
#include <memory>

namespace ngl
{
	class actor_base;
	using ptractor = std::shared_ptr<actor_base>;

	struct nnode_session
	{
		i32_sessionid	m_session = -1;	// 服务器session
		nactornode		m_node;		    // 服务器基本信息

		nnode_session() = default;
		nnode_session(i32_sessionid asession, const nactornode& anode);
		explicit nnode_session(const nactornode& anode);
	};

	struct handle_pram;

	template <typename T>
	struct handle_send
	{
		static bool send_server(i32_serverid aserverid, const nguid& aactorid, const nguid& arequestactorid, const T& adata);
		static bool send_server(i32_serverid aserverid, const handle_pram& adata);
		static bool send_client(handle_pram& adata);

		static bool send(handle_pram& adata);
	};

	struct handle_pram
	{
		handle_pram(handle_pram&&) noexcept = default;
		handle_pram& operator=(handle_pram&&) noexcept = default;
		handle_pram(const handle_pram&) = default;
		handle_pram& operator=(handle_pram&) = default;
		handle_pram() = default;

		i32_protocolnum			m_enum			= -1;				// 协议号
		std::shared_ptr<void>	m_data			= nullptr;			// 协议结构
		std::shared_ptr<pack>	m_pack			= nullptr;			// 如果是网络消息会携带pack信息
		nguid					m_actor			= nguid::make();	// 发送给哪个actor
		nguid					m_requestactor	= nguid::make();	// 哪个actor发送的
		std::set<i64_actorid>   m_massactors;						// 群发列表

		using forwardtype = std::function<void(handle_pram&)>;
		using callfail = std::function<void()>;

		forwardtype				m_forward		= nullptr;			// 转发函数
		bool					m_forwardtype	= false;			// 转发给所有类型
		callfail				m_failfun		= nullptr;			// 如何actor_client都找不到目标actor则调用
		bool					m_issend		= true;				// 是否会发送给其他进程

		//# 根据[连接]获取[id]
		static i32_serverid		serverid(i64_actorid aactorid);

		//# 根据[actorid]获取[gatewayid]
		static i32_serverid		gatewayid(i64_actorid aactorid);

		//# 根据[服务器类型]获取[服务器列表]
		static void				serveridlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec);

		//# 是否是无效的actor guid
		static bool				is_actoridnone(const nguid& aguid);

		//# 通过session发送消息T
		template <typename T>
		static bool	send(i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid);
		
		//# 向服务器发送pack
		static bool	send_pack(i32_serverid aserverid, std::shared_ptr<pack>& apack);
		static bool	send_pack(i32_serverid aserverid, std::shared_ptr<void>& apack);
		
		template <typename T, bool IS_SEND = true>
		static void	make_forward(handle_pram& apram)
		{
			static auto lfun = [](handle_pram& adata)
			{
				handle_send<T>::send(adata);
			};
			if constexpr (IS_SEND)
			{
				apram.m_forward = lfun;
			}
		}

		template <typename T>
		static void	make_client(handle_pram& apram)
		{
			static auto lfun = [](handle_pram& adata)
			{
				handle_send<T>::send_client(adata);
			};
			apram.m_forward = lfun;
		}

		template <typename T, bool IS_SEND = true, bool IS_FORWARDFUN = true>
		static handle_pram create(const nguid& aid, const nguid& arid, const std::shared_ptr<T>& adata, const callfail& afailfun = nullptr)
		{
			handle_pram lpram;
			lpram.m_enum = tprotocol::protocol<T>();
			lpram.m_data = adata;
			lpram.m_actor = aid;
			lpram.m_requestactor = arid;
			lpram.m_issend = IS_SEND;
			make_forward<T, IS_FORWARDFUN&& IS_SEND>(lpram);
			lpram.m_failfun = afailfun;
			return lpram;
		}

		template <typename T, bool IS_SEND = true>
		static handle_pram create(const std::set<i64_actorid>& aids, const nguid& arid, const std::shared_ptr<T>& adata, const callfail& afailfun = nullptr)
		{
			handle_pram lpram;
			lpram.m_enum = tprotocol::protocol<T>();
			lpram.m_data = adata;
			lpram.m_massactors = aids;
			lpram.m_requestactor = arid;
			lpram.m_issend = IS_SEND;
			make_forward<T, IS_SEND>(lpram);
			lpram.m_failfun = afailfun;
			return lpram;
		}

		template <typename T, typename Y>
		using nforward_g2c = std::shared_ptr<np_actor_forward<T, forward_g2c<Y>>>;

		//Y:forward或者T
		template <typename T, typename Y>
		static handle_pram create(const nguid& aid, const nguid& arid, const nforward_g2c<T, Y>& adata, const callfail& afailfun = nullptr)
		{
			handle_pram lpram;
			lpram.m_enum = tprotocol::protocol<T>();
			lpram.m_data = adata;
			lpram.m_actor = aid;
			lpram.m_requestactor = arid;
			make_client<T>(lpram);
			lpram.m_failfun = afailfun;
			return lpram;
		}

		template <typename T, typename Y>
		using nforward_c2g = std::shared_ptr<np_actor_forward<T, forward_c2g<Y>>>;

		template <typename T, typename Y>
		static handle_pram create(const nguid& aid, const nguid& arid, const nforward_c2g<T, Y>& adata, const callfail& afailfun = nullptr)
		{
			handle_pram lpram;
			lpram.m_enum = tprotocol::protocol<T>();
			lpram.m_data = adata;
			lpram.m_actor = aid;
			lpram.m_requestactor = arid;
			lpram.m_issend = true;
			lpram.m_failfun = afailfun;
			return lpram;
		}

		static handle_pram create(const nguid& aid, const nguid& arid, const std::shared_ptr<pack>& apack)
		{
			handle_pram lpram;
			lpram.m_data = apack;
			lpram.m_actor = aid;
			lpram.m_requestactor = arid;
			make_forward<pack, true>(lpram);
			return lpram;
		}

		template <typename Y>
		static bool send(nguid& aactorid, handle_pram& adata)
		{
			i32_serverid lserverid = handle_pram::serverid(aactorid);
			if (lserverid == -1)
			{
				if (adata.m_forwardtype && handle_pram::is_actoridnone(aactorid))
				{//# 转发给所有类型为nguid::type(aactorid)的actor
					std::set<i32_serverid> lserverids;
					handle_pram::serveridlist(aactorid.type(), lserverids);
					for (i32_serverid serverid : lserverids)
					{
						handle_send<Y>::send_server(serverid, adata);
					}
					return true;
				}
				if (adata.m_failfun != nullptr)
				{
					adata.m_failfun();
				}
				return false;
			}
			return handle_send<Y>::send_server(lserverid, adata);
		}
	};

	template <typename T>
	bool handle_send<T>::send(handle_pram& adata)
	{
		nguid lactorid = adata.m_actor;
		nguid lrequestactor = adata.m_requestactor;
		std::set<i64_actorid>& lmassactors = adata.m_massactors;
		if (lactorid != nguid::make() && lmassactors.empty())
		{
			return handle_pram::send<T>(lactorid, adata);
		}
		else if (lactorid == nguid::make() && !lmassactors.empty())
		{
			std::map<i32_serverid, std::set<i64_actorid>> lservers;
			for (i64_actorid actorid : lmassactors)
			{
				i32_serverid lserverid = handle_pram::serverid(actorid);
				if (lserverid != -1 && lserverid != nconfig.nodeid())
				{
					lservers[lserverid].insert(actorid);
				}
			}
			std::shared_ptr<T> ldata = std::static_pointer_cast<T>(adata.m_data);
			for (auto& [_serverid, _actorids] : lservers)
			{
				np_mass_actor<T> pro(ldata);
				pro.m_actorids.swap(_actorids);
				handle_send<np_mass_actor<T>>::send_server(_serverid, nguid::make(), lrequestactor, pro);
			}
		}
		return false;
	}

	template <>
	class handle_send<pack>
	{
	public:
		static bool send_server(i32_serverid aserverid, handle_pram& adata)
		{
			return handle_pram::send_pack(server_session::sessionid(aserverid), adata.m_data);
		}

		static bool send(handle_pram& adata)
		{
			return handle_pram::send<pack>(adata.m_actor, adata);
		}
	};

	template <typename T>
	bool handle_send<T>::send_client(handle_pram& adata)
	{
		auto ldata = (np_actor_forward<T, forward_g2c<T>>*)adata.m_data.get();
		std::vector<i32_actordataid>& luid = ldata->m_data.m_uid;
		std::vector<i16_area>& lareas = ldata->m_data.m_area;
		std::set<i32_serverid> lgateways;
		for (int i = 0; i < luid.size() && i < lareas.size(); ++i)
		{
			i64_actorid lroleactor = nguid::make(ACTOR_ROLE, lareas[i], luid[i]);
			i32_serverid lserverid = handle_pram::gatewayid(lroleactor);
			if (lserverid > 0)
			{
				lgateways.insert(lserverid);
			}
		}
		for (i32_serverid lserverid : lgateways)
		{
			handle_send<np_actor_forward<T, forward_g2c<T>>>::send_server(lserverid, adata);
		}
		return true;
	}
}//namespace ngl