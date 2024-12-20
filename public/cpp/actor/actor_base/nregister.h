#pragma once

#include "nactortype.h"
#include "actor_base.h"
#include "tprotocol.h"
#include "nprotocol.h"
#include "protocol.h"
#include "actor.h"

namespace ngl
{
	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename TTTDerived, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun(const std::function<void(TTTDerived*, message<T>&)>& afun)
	{
		m_fun[tprotocol::protocol<T>()] = nlogicfun
		{
			.m_isdbload = false,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				std::shared_ptr<T> lptr = std::static_pointer_cast<T>(apram.m_data);
				message<T> lmessage(athreadid, apram.m_pack.get(), lptr);
				afun((TTTDerived*)aactor, lmessage);
			}
		};
		protocol::registry_actor<T, TYPE>(nactor_type<TDerived>::type(), tprotocol::protocol_name<T>().c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename TTTDerived, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun_nonet(const Tfun<TTTDerived, T> afun, bool aisload/* = false*/)
	{
		m_fun[tprotocol::protocol<T>()] = nlogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				std::shared_ptr<T> lptr = std::static_pointer_cast<T>(apram.m_data);
				message<T> lmessage(athreadid, apram.m_pack.get(), lptr);
				(((TTTDerived*)(aactor))->*afun)(lmessage);
			}
		};
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename TTTDerived, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun(const Tfun<TTTDerived, T> afun, bool aisload/* = false*/)
	{
		rfun<TTTDerived, T>(afun, nactor_type<TDerived>::type(), aisload);
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename TTTDerived, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun(const Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, bool aisload/* = false*/)
	{
		rfun_nonet<TTTDerived, T>(afun, aisload);
		protocol::registry_actor<T, TYPE>(atype, tprotocol::protocol_name<T>().c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <bool BOOL, typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun_forward(const Tfun<TDerived, np_actor_forward<T, TYPE, BOOL, ngl::forward>> afun, ENUM_ACTOR atype, bool aisload/* = false*/)
	{
		using type_forward = np_actor_forward<T, TYPE, BOOL, ngl::forward>;
		m_fun[tprotocol::protocol<type_forward>()] = nlogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				std::shared_ptr<type_forward> lptr = std::static_pointer_cast<type_forward>(apram.m_data);
				message<type_forward> lmessage(athreadid, apram.m_pack.get(), lptr);
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};

		int32_t lprotocol		= tprotocol::protocol<type_forward>();
		const std::string lname = tprotocol::protocol_name<type_forward>();
		protocol::registry_actor_forward<T, type_forward::isusing, TYPE>(atype, lprotocol, lname.c_str());
		return *this;
	}

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	template <typename T>
	nrfun<TDerived, TYPE>& nrfun<TDerived, TYPE>::rfun_recvforward(const Tfun<TDerived, T> afun, bool aisload/* = false*/)
	{
		using type_forward = np_actor_forward<T, TYPE, false, T>;
		m_fun[tprotocol::protocol<type_forward>()] = nlogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				auto ltemp = (type_forward*)apram.m_data.get();
				std::shared_ptr<T> pro = ltemp->get_shared();
				message<T> lmessage(athreadid, apram.m_pack.get(), pro);
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};

		ENUM_ACTOR ltype		= nactor_type<TDerived>::type();
		int32_t lprotocol		= tprotocol::protocol<type_forward>();
		const std::string lname = tprotocol::protocol_name<type_forward>();
		protocol::registry_actor_recvforward<T, type_forward::isusing, TYPE>(ltype, lprotocol, lname.c_str());
		return *this;
	}
}//namespace ngl