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
// File overview: Declares interfaces for protocol.

#pragma once

#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/handle_pram.h"
#include "tools/serialize/structbytes.h"
#include "actor/actor_base/nactortype.h"
#include "actor/protocol/nprotocol.h"
#include "actor/protocol/tprotocol.h"
#include "actor/actor_base/nguid.h"
#include "tools/serialize/pack.h"
#include "tools/threadtools.h"
#include "tools/log/nlog.h"
#include "net/tcp/ntcp.h"
#include "tools/type.h"

#include <functional>
#include <map>

namespace ngl
{
	enum eprotocol_tar
	{};

	class protocol
	{
		protocol() = delete;
		protocol(const protocol&) = delete;
		protocol& operator=(const protocol&) = delete;

	public:
		using fun_pack = std::function<std::shared_ptr<void>(std::shared_ptr<pack>&)>;
		using fun_run = std::function<bool(std::shared_ptr<pack>&, std::shared_ptr<void>&)>;
	private:
		struct pfun
		{
			protocol::fun_pack								m_packfun = nullptr;		// Deserializer from pack -> typed payload.
			std::map<ENUM_ACTOR, protocol::fun_run>			m_runfun;					// Dispatch entry points keyed by actor type.
		};
		static std::map<i32_protocolnum, protocol::pfun>	m_protocolfun;	// Protocol id -> deserializer + dispatch table.
		static std::shared_mutex							m_mutex;		// Protects the protocol registry.

	public:
		// Find one registered protocol entry and resolve its display name for logging.
		static pfun* find(i32_protocolnum aprotocol);

		static const char* name(i32_protocolnum aprotocol);

		static void print(const char* amsg, i32_protocolnum aprotocol);

		// Register one protocol id for one actor type.
		static void registers(
			int aprotocol, ENUM_ACTOR aenumactor, const protocol::fun_pack& apackfun, const protocol::fun_run& arunfun, const char* aname
		);

		// Decode a network pack and route it to the matching actor dispatcher.
		static void push(std::shared_ptr<pack>& apack);

		// Register a normal actor message: deserialize T and enqueue it by actor guid/type.
		template <typename T>
		static void registry_actor(ENUM_ACTOR atype, const char* aname)
		{
			fun_pack lpackfun = [atype](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
			{
				Try
				{
					auto lp = std::make_shared<T>();
					std::shared_ptr<void> ltemp = std::static_pointer_cast<void>(lp);
					if (structbytes<T>::tostruct(apack, *lp))
					{
						return ltemp;
					}
				}Catch
				return nullptr;
			};
			fun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
			{
				nguid lactorguid(apack->m_head.get_actor());
				nguid lrequestactorguid(apack->m_head.get_request_actor());
				std::shared_ptr<T> ldatapack = std::static_pointer_cast<T>(aptrpram);
				handle_pram lpram = handle_pram::create<T, false, false>(lactorguid, lrequestactorguid, ldatapack);
				lpram.m_pack = apack;

				actor_manage& lmanages = actor_manage::instance();

				if (nconfig.nodetype() == ROBOT && apack->m_head.get_actor() == nguid::make())
				{
					// Robot-originated packets can broadcast by actor type without a concrete target id.
					lmanages.push_task_type(atype, lpram);
					return true;
				}

				if (lactorguid.is_actortypenone() || lactorguid.is_moreactor(atype))
				{
					// Missing type or "moreactor" means deliver to every local actor of this type.
					lmanages.push_task_type(atype, lpram);
					return true;
				}
				else
				{
					if (lactorguid.type() == atype)
					{
						if (lactorguid.is_actoridnone())
						{
							// Type-only address means broadcast within the local process.
							lmanages.push_task_type(atype, lpram);
						}							
						else
						{
							lmanages.push_task_id(lactorguid.id(), lpram);
						}
					}
				}
				return true;
			};
			registers(tprotocol::protocol<T>(), atype, lpackfun, lrunfun, aname);
		}

		// Register a client -> gateway -> server forwarding protocol.
		template <typename T>
		static void registry_c2g(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						auto lp = std::make_shared<np_actor_forward<T, forward_c2g<forward>>>();
						std::shared_ptr<void> ltemp = std::static_pointer_cast<void>(lp);
						if (structbytes<np_actor_forward<T, forward_c2g<forward>>>::tostruct(apack, *lp, true))
						{
							return ltemp;
						}
					}Catch
					return nullptr;
				};
			fun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
				{
					auto ldatapack = std::static_pointer_cast<np_actor_forward<T, forward_c2g<forward>>>(aptrpram);
					nguid lguid(atype, tab_self_area, nconfig.tcount());
					nguid lrequestguid(apack->m_head.get_request_actor());
					handle_pram lpram = handle_pram::create<T, forward>(lguid, lrequestguid, ldatapack);
					lpram.m_pack = apack;
					actor_manage::instance().push_task_id(lguid, lpram);
					return true;
				};
			registers(aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		// Register a server -> gateway -> client forwarding protocol.
		template <typename T>
		static void registry_g2c(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						auto lp = std::make_shared<np_actor_forward<T, forward_g2c<forward>>>();
						std::shared_ptr<void> ltemp = std::static_pointer_cast<void>(lp);
						if (structbytes<np_actor_forward<T, forward_g2c<forward>>>::tostruct(apack, *lp, true))
						{
							return ltemp;
						}
					}Catch
					return nullptr;
				};
			fun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
				{
					auto ldatapack = std::static_pointer_cast<np_actor_forward<T, forward_g2c<forward>>>(aptrpram);
					nguid lguid(atype, tab_self_area, nconfig.tcount());
					nguid lrequestguid(apack->m_head.get_request_actor());
					handle_pram lpram = handle_pram::create<np_actor_forward<T, forward_g2c<forward>>>(lguid, lrequestguid, ldatapack);
					lpram.m_pack = apack;
					actor_manage::instance().push_task_id(lguid, lpram);
					return true;
				};
			registers(aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		// Register the synthetic mass-send wrapper used to address many actors at once.
		template <typename T>
		static void registry_mass(int32_t aprotocolnum, const char* aname)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						auto lp = std::make_shared<np_mass_actor<T>>();
						std::shared_ptr<void> ltemp(lp);
						if (structbytes<np_mass_actor<T>>::tostruct(apack, *lp))
						{
							return ltemp;
						}
					}Catch
					return nullptr;
				};
			fun_run lrunfun = [](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
				{
					std::shared_ptr<np_mass_actor<T>> ldatapack = std::static_pointer_cast<np_mass_actor<T>>(aptrpram);
					std::set<i64_actorid>& lactorids = ldatapack->m_actorids;
					nguid lrequestguid(apack->m_head.get_request_actor());

					std::shared_ptr<T> ldata = ldatapack->shared_data();
					
					handle_pram lpram = handle_pram::create<T, false>(lactorids, lrequestguid, ldata);
					actor_manage::instance().push_task_id(lactorids, lpram);
					return true;
				};
			registers(aprotocolnum, (ENUM_ACTOR)nguid::none_type(), lpackfun, lrunfun, aname);
		}

		// Handle plain-text telnet admin commands.
		static void telnet_cmd(const std::shared_ptr<pack>& apack);
	};
}// namespace ngl
