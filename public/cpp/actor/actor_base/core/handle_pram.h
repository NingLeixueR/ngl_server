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
// File overview: Message dispatch parameter block carrying sender, receiver, and payload.

#pragma once

#include "actor/actor_base/core/nguid.h"
#include "actor/protocol/tprotocol.h"
#include "actor/protocol/nprotocol.h"
#include "net/server_session.h"
#include "tools/type.h"

#include <cstdint>
#include <memory>

namespace ngl
{
	class actor_base;
	using ptractor = std::shared_ptr<actor_base>;

	struct nnode_session
	{
		i32_sessionid	m_session = -1;	// Connected session id.
		nactornode		m_node;		    // Remote node metadata.

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
		handle_pram& operator=(const handle_pram&) = default;
		handle_pram() = default;

		i32_protocolnum			m_enum			= -1;				// Protocol id
		std::shared_ptr<void>	m_data			= nullptr;			// Typed protocol payload.
		std::shared_ptr<pack>	m_pack			= nullptr;			// Raw network packet when forwarding directly.
		nguid					m_actor			= nguid::make();	// Target actor.
		nguid					m_requestactor	= nguid::make();	// Original sender actor.
		std::set<i64_actorid>   m_massactors;						// Batch targets for mass forwarding.

		using forwardtype = std::function<void(handle_pram&)>;
		using callfail = std::function<void()>;

		forwardtype				m_forward		= nullptr;			// Transport callback used by remote forwarding.
		bool					m_forwardtype	= false;			// Broadcast by actor type when no concrete guid exists.
		callfail				m_failfun		= nullptr;			// Failure callback when routing cannot resolve a target.
		bool					m_issend		= true;				// Whether this message should be sent remotely at all.

		// Build a lightweight copy for local fan-out without duplicating the
		// potentially large mass-target set.
		static handle_pram shallow_copy_without_massactors(const handle_pram& asrc)
		{
			handle_pram ldst;
			ldst.m_enum			= asrc.m_enum;
			ldst.m_data			= asrc.m_data;
			ldst.m_pack			= asrc.m_pack;
			ldst.m_actor		= asrc.m_actor;
			ldst.m_requestactor	= asrc.m_requestactor;
			ldst.m_forward		= asrc.m_forward;
			ldst.m_forwardtype	= asrc.m_forwardtype;
			ldst.m_failfun		= asrc.m_failfun;
			ldst.m_issend		= asrc.m_issend;
			return ldst;
		}

		// Resolve an actor guid to the owning server id.
		static i32_serverid		serverid(i64_actorid aactorid);

		// Resolve a role actor to the gateway server that currently owns it.
		static i32_serverid		gatewayid(i64_actorid aactorid);

		// Collect all server ids for a given singleton/system actor type.
		static void				serveridlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec);

		// True when the guid does not point to a concrete actor instance.
		static bool				is_actoridnone(const nguid& aguid);

		// Send one typed payload through an existing TCP session.
		template <typename T>
		static bool	send(i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid);
		
		// Send one already-built packet to the server that owns the route.
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

		// Y:forwardor T
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
				{
					// Broadcast to every server that hosts the requested actor type
					// when the sender intentionally omitted a concrete actor id.
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
			for (const i64_actorid lmassactorid : lmassactors)
			{
				i32_serverid lserverid = handle_pram::serverid(lmassactorid);
				if (lserverid != -1 && lserverid != nconfig.nodeid())
				{
					lservers[lserverid].insert(lmassactorid);
				}
			}
			std::shared_ptr<T> ldata = std::static_pointer_cast<T>(adata.m_data);
			for (auto& lpair : lservers)
			{
				np_mass_actor<T> pro(ldata);
				pro.m_actorids.swap(lpair.second);
				handle_send<np_mass_actor<T>>::send_server(lpair.first, nguid::make(), lrequestactor, pro);
			}
			return true;
		}
		return false;
	}

	template <>
	class handle_send<pack>
	{
	public:
		static bool send_server(i32_serverid aserverid, handle_pram& adata)
		{
			return handle_pram::send_pack(aserverid, adata.m_data);
		}

		static bool send(handle_pram& adata)
		{
			return handle_pram::send<pack>(adata.m_actor, adata);
		}
	};

	template <typename T>
	bool handle_send<T>::send_client(handle_pram& adata)
	{
		auto* ldata = static_cast<np_actor_forward<T, forward_g2c<T>>*>(adata.m_data.get());
		if (ldata == nullptr)
		{
			return false;
		}
		std::vector<i32_actordataid>& luid = ldata->m_data.m_uid;
		std::vector<i16_area>& lareas = ldata->m_data.m_area;
		std::set<i32_serverid> lgateways;
		const std::size_t lcount = luid.size() < lareas.size() ? luid.size() : lareas.size();
		for (std::size_t li = 0; li < lcount; ++li)
		{
			i64_actorid lroleactor = nguid::make(ACTOR_ROLE, lareas[li], luid[li]);
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
