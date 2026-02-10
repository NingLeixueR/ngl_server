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
	nrfun<TDerived>& nrfun<TDerived>::rfun(const std::function<void(TTTDerived*, message<T>&)>& afun, int32_t aready /*= e_ready_all*/)
	{	
		register_logic(tprotocol::protocol<T>(), aready, [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
				message lmessage(athreadid, apram.m_pack, std::static_pointer_cast<T>(apram.m_data));
				afun((TTTDerived*)aactor, lmessage);
				if (aactor->ready().is_ready(e_ready_db))
				{
					((TTTDerived*)aactor)->handle_after(apram);
				}
			}
		);
		protocol::registry_actor<T>(nactor_type<TDerived>::type(), tools::type_name<T>().c_str());
		if constexpr (!is_protobuf_message<T>::value)
		{
			protocol::registry_actor_mass<T>(tprotocol::protocol<np_mass_actor<T>>(), tools::type_name<T>().c_str());
		}
		return *this;
	}

	template <typename TDerived>
	template <typename TTTDerived, typename T>
	nrfun<TDerived>& nrfun<TDerived>::rfun_nonet(const Tfun<TTTDerived, T> afun, int32_t aready /*= e_ready_all*/)
	{
		register_logic(tprotocol::protocol<T>(), aready, [afun](actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
			{
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
				message lmessage(athreadid, apram.m_pack, (type_forward_c2g*)apram.m_data.get());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		);
		protocol::registry_actor_c2g<T>(nactor_type<TDerived>::type(), tprotocol::protocol<type_forward_c2g>(), tools::type_name<type_forward_c2g>().c_str());
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
				message lmessage(athreadid, apram.m_pack, (type_forward_g2c*)apram.m_data.get());
				(((TDerived*)(aactor))->*afun)(lmessage);
			}
		);
		protocol::registry_actor_g2c<T>(nactor_type<TDerived>::type(), tprotocol::protocol<type_forward_g2c>(), tools::type_name<type_forward_g2c>().c_str());
		return *this;
	}
}//namespace ngl