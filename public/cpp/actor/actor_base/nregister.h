#pragma once

#include "nactortype.h"
#include "actor_base.h"
#include "tprotocol.h"
#include "nprotocol.h"
#include "protocol.h"
#include "actor.h"

namespace ngl
{
	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun(const std::function<void(TTTDerived*, message<T>&)>& afun, bool aisload /*= false*/)
	{	
		m_fun[tprotocol::protocol<T>()] = nlogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				std::shared_ptr<T> ldata = std::static_pointer_cast<T>(apram.m_data);
				message<T> lmessage(athreadid, apram.m_pack.get(), ldata);
				afun((TTTDerived*)aactor, lmessage);
			}
		};
		protocol::registry_actor<T>(nactor_type<TDerived>::type(), tools::type_name<T>().c_str());
		return *this;
	}

	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun_nonet(const Tfun<TTTDerived, T> afun, bool aisload/* = false*/)
	{
		m_fun[tprotocol::protocol<T>()] = nlogicfun
		{
			.m_isdbload = aisload,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				std::shared_ptr<T> ldata = std::static_pointer_cast<T>(apram.m_data);
				message<T> lmessage(athreadid, apram.m_pack.get(), ldata);
				(((TTTDerived*)(aactor))->*afun)(lmessage);
			}
		};
		return *this;
	}

	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun(const Tfun<TTTDerived, T> afun, bool aisload/* = false*/)
	{
		rfun<TTTDerived, T>(afun, nactor_type<TDerived>::type(), aisload);
		return *this;
	}
	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun(const Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, bool aisload/* = false*/)
	{
		rfun_nonet<TTTDerived, T>(afun, aisload);
		protocol::registry_actor<T>(atype, tools::type_name<T>().c_str());
		return *this;
	}

	template <typename TDerived>
	template <typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun_c2g(const Tfun<TDerived, np_actor_forward<T, forward_c2g<forward>>> afun)
	{
		using type_forward_c2g = np_actor_forward<T, forward_c2g<forward>>;
		m_fun[tprotocol::protocol<np_actor_forward<T, forward_c2g<forward>>>()] = nlogicfun
		{
			.m_isdbload = false,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				type_forward_c2g* ltemp = (type_forward_c2g*)apram.m_data.get();
				message<type_forward_c2g> lmessage(athreadid, apram.m_pack.get(), ltemp);
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};
		protocol::registry_actor_c2g<T>(
			nactor_type<TDerived>::type()
			, tprotocol::protocol<type_forward_c2g>()
			, tools::type_name<type_forward_c2g>().c_str()
		);
		return *this;
	}

	template <typename TDerived>
	template <typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun_g2c(const Tfun<TDerived, np_actor_forward<T, forward_g2c<forward>>> afun)
	{
		using type_forward_g2c = np_actor_forward<T, forward_g2c<forward>>;
		m_fun[tprotocol::protocol<np_actor_forward<T, forward_g2c<forward>>>()] = nlogicfun
		{
			.m_isdbload = false,
			.m_fun = [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				type_forward_g2c* ltemp = (type_forward_g2c*)apram.m_data.get();
				message<type_forward_g2c> lmessage(athreadid, apram.m_pack.get(), ltemp);
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		};
		protocol::registry_actor_g2c<T>(
			nactor_type<TDerived>::type()
			, tprotocol::protocol<type_forward_g2c>()
			, tools::type_name<type_forward_g2c>().c_str()
		);
		return *this;
	}
}//namespace ngl