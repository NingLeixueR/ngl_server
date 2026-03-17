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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/actor_base/nactortype.h"
#include "actor/actor_base/actor_base.h"
#include "actor/protocol/tprotocol.h"
#include "actor/protocol/nprotocol.h"
#include "actor/protocol/protocol.h"
#include "actor/actor_base/actor.h"

namespace ngl
{
	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun(const std::function<void(TTTDerived*, message<T>&)>& afun, int32_t aready /*= e_ready_all*/)
	{	
		register_logic(tprotocol::protocol<T>(), aready, [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				// Shared_ptr-backed messages are safe to hand directly to higher-level handlers.
				message lmessage(athreadid, apram.m_pack, std::static_pointer_cast<T>(apram.m_data));
				afun((TTTDerived*)aactor, lmessage);
				if (aactor->ready().is_ready(e_ready_db))
				{
					// Post-handle hooks run only after DB-dependent state is ready.
					((TTTDerived*)aactor)->handle_after(apram);
				}
			}
		);
		// Registry metadata is used by the protocol reflection / routing layers.
		protocol::registry_actor<T>(nactor_type<TDerived>::type(), tools::type_name<T>().c_str());
		if constexpr (!is_protobuf_message<T>::value)
		{
			protocol::registry_mass<T>(tprotocol::protocol<np_mass_actor<T>>(), tools::type_name<T>().c_str());
		}
		return *this;
	}

	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun_nonet(const Tfun<TTTDerived, T> afun, int32_t aready /*= e_ready_all*/)
	{
		register_logic(tprotocol::protocol<T>(), aready, [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				// Non-network handlers still reuse the same message envelope and post-handle flow.
				message lmessage(athreadid, apram.m_pack, std::static_pointer_cast<T>(apram.m_data));
				(((TTTDerived*)(aactor))->*afun)(lmessage);
				if (aactor->ready().is_ready(e_ready_db))
				{
					((TTTDerived*)aactor)->handle_after(apram);
				}
			}
		);
		return *this;
	}

	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun(const Tfun<TTTDerived, T> afun, int32_t aready /*= e_ready_all*/)
	{
		rfun<TTTDerived, T>(afun, nactor_type<TDerived>::type(), aready);
		return *this;
	}

	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun(const Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, int32_t aready /*= e_ready_all*/)
	{
		rfun_nonet<TTTDerived, T>(afun, aready);
		protocol::registry_actor<T>(atype, tools::type_name<T>().c_str());
		return *this;
	}

	template <typename TDerived>
	template <typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun_c2g(const Tfun<TDerived, np_actor_forward<T, forward_c2g<forward>>> afun)
	{
		using type_forward_c2g = np_actor_forward<T, forward_c2g<forward>>;
		register_logic(tprotocol::protocol<np_actor_forward<T, forward_c2g<forward>>>(), e_ready_null
			, [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				// Forward packets already contain the wrapped payload, so no extra copy is needed.
				message lmessage(athreadid, apram.m_pack, (type_forward_c2g*)apram.m_data.get());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		);
		protocol::registry_c2g<T>(nactor_type<TDerived>::type(), tprotocol::protocol<type_forward_c2g>(), tools::type_name<type_forward_c2g>().c_str());
		return *this;
	}

	template <typename TDerived>
	template <typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun_g2c(const Tfun<TDerived, np_actor_forward<T, forward_g2c<forward>>> afun)
	{
		using type_forward_g2c = np_actor_forward<T, forward_g2c<forward>>;
		register_logic(tprotocol::protocol<np_actor_forward<T, forward_g2c<forward>>>(), e_ready_null
			, [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				// Forward packets already contain the wrapped payload, so no extra copy is needed.
				message lmessage(athreadid, apram.m_pack, (type_forward_g2c*)apram.m_data.get());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		);
		protocol::registry_g2c<T>(nactor_type<TDerived>::type(), tprotocol::protocol<type_forward_g2c>(), tools::type_name<type_forward_g2c>().c_str());
		return *this;
	}
}//namespace ngl
