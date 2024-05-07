#pragma once

#include "actor_base.h"
#include "actor.h"
#include "nactor_type.h"
#include "nprotocol.h"
#include "tprotocol.h"
#include "protocol.h"
#include "regular.h"

namespace ngl
{
	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename TTTDerived, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun(const std::function<void(TTTDerived*, T&)>& afun)
	{
		m_fun[tprotocol::protocol<T>()] = nlogicfun
		{
			.m_isdbload = false,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				afun((TTTDerived*)aactor, *(T*)apram.m_data.get());
			}
		};
		protocol::registry_actor<T, TYPE>(nactor_type<TDerived>::type(), tprotocol::protocol_name<T>().c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename TTTDerived, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun_nonet(Tfun<TTTDerived, T> afun, bool aisload/* = false*/)
	{
		m_fun[tprotocol::protocol<T>()] = nlogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				message<T> lmessage(athreadid, apram.m_pack.get(), (T*)apram.m_data.get());
				(((TTTDerived*)(aactor))->*afun)(lmessage);
			}
		};
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename TTTDerived, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun(Tfun<TTTDerived, T> afun, bool aisload/* = false*/)
	{
		rfun<TTTDerived, T>(afun, nactor_type<TDerived>::type(), aisload);
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename TTTDerived, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun(Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, bool aisload/* = false*/)
	{
		rfun_nonet<TTTDerived, T>(afun, aisload);
		protocol::registry_actor<T, TYPE>(atype, tprotocol::protocol_name<T>().c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <bool BOOL, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun_forward(Tfun<TDerived, np_actor_forward<T, TYPE, BOOL, ngl::forward>> afun, ENUM_ACTOR atype, bool aisload/* = false*/)
	{
		using type_forward = np_actor_forward<T, TYPE, BOOL, ngl::forward>;
		m_fun[tprotocol::protocol<type_forward>()] = nlogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				message<type_forward> lmessage(athreadid, apram.m_pack.get(), (type_forward*)apram.m_data.get());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};
		protocol::registry_actor_forward<T, type_forward::isusing, TYPE>(atype, tprotocol::protocol<type_forward>(), tprotocol::protocol_name<type_forward>().c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun_recvforward(Tfun<TDerived, T> afun, bool aisload/* = false*/)
	{
		using type_forward = np_actor_forward<T, TYPE, false, T>;
		m_fun[tprotocol::protocol<type_forward>()] = nlogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				type_forward* ltemp = (type_forward*)apram.m_data.get();
				message<T> lmessage(athreadid, apram.m_pack.get(), ltemp->get_data());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};
		protocol::registry_actor_recvforward<T, type_forward::isusing, TYPE>(nactor_type<TDerived>::type(), tprotocol::protocol<type_forward>(), tprotocol::protocol_name<type_forward>().c_str());
		return *this;
	}
}//namespace ngl
