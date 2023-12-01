#pragma once

#include "protocol.h"
#include "actor_base.h"
#include "actor_protocol.h"
#include "init_protobuf.h"
#include "regular.h"

namespace ngl
{
	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename T>
	arfun<TDerived, TYPE>& arfun<TDerived, TYPE>::rfun(const std::function<void(TDerived*, T&)>& afun)
	{
		m_fun[init_protobuf::protocol<T>()] = alogicfun
		{
			.m_isdbload = false,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				afun((TDerived*)aactor, *(T*)apram.m_data.get());
			}
		};
		protocol::registry_actor<T, TYPE>((ENUM_ACTOR)TDerived::ACTOR_TYPE, init_protobuf::protocol_name<T>().c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename T>
	arfun<TDerived, TYPE>& arfun<TDerived, TYPE>::rfun_nonet(Tfun<TDerived, T> afun, bool aisload/* = false*/)
	{
		m_fun[init_protobuf::protocol<T>()] = alogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				message<T> lmessage(athreadid, apram.m_pack.get(), (T*)apram.m_data.get());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename T>
	arfun<TDerived, TYPE>& arfun<TDerived, TYPE>::rfun(Tfun<TDerived, T> afun, bool aisload/* = false*/)
	{
		rfun(afun, (ENUM_ACTOR)TDerived::ACTOR_TYPE, aisload);
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename T>
	arfun<TDerived, TYPE>& arfun<TDerived, TYPE>::rfun(Tfun<TDerived, T> afun, ENUM_ACTOR atype, bool aisload/* = false*/)
	{
		rfun_nonet(afun, aisload);
		protocol::registry_actor<T, TYPE>(atype, init_protobuf::protocol_name<T>().c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <bool BOOL, typename T>
	arfun<TDerived, TYPE>& arfun<TDerived, TYPE>::rfun_forward(Tfun<TDerived, actor_forward<T, TYPE, BOOL, ngl::forward>> afun, ENUM_ACTOR atype, bool aisload/* = false*/)
	{
		using type_forward = actor_forward<T, TYPE, BOOL, ngl::forward>;
		m_fun[init_protobuf::protocol<type_forward>()] = alogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				message<type_forward> lmessage(athreadid, apram.m_pack.get(), (type_forward*)apram.m_data.get());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};
		protocol::registry_actor_forward<T, type_forward::isusing, TYPE>(atype, init_protobuf::protocol<type_forward>(), init_protobuf::protocol_name<type_forward>().c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename T>
	arfun<TDerived, TYPE>& arfun<TDerived, TYPE>::rfun_recvforward(Tfun<TDerived, T> afun, bool aisload/* = false*/)
	{
		using type_forward = actor_forward<T, TYPE, false, T>;
		m_fun[init_protobuf::protocol<type_forward>()] = alogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				type_forward* ltemp = (type_forward*)apram.m_data.get();
				message<T> lmessage(athreadid, apram.m_pack.get(), ltemp->get_data());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};
		protocol::registry_actor_recvforward<T, type_forward::isusing, TYPE>((ENUM_ACTOR)TDerived::ACTOR_TYPE, init_protobuf::protocol<type_forward>(), init_protobuf::protocol_name<type_forward>().c_str());
		return *this;
	}
}
