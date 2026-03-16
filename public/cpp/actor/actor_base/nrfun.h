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

#include "actor/actor_base/handle_pram.h"
#include "actor/actor_base/actor_base.h"
#include "tools/type.h"

#include <functional>
#include <map>

namespace ngl
{
	using tlogicfun		= std::function<void(actor_base*, i32_threadid, handle_pram&)>;
	using tnotfindfun	= std::function<void(i32_threadid, handle_pram&)>;

	struct nlogicfun
	{
		int32_t m_ready = e_ready_all;
		tlogicfun m_fun = nullptr;
	};

	class nrfunbase
	{
		nrfunbase(const nrfunbase&) = delete;
		nrfunbase& operator=(const nrfunbase&) = delete;

		std::map<i32_protocolnum, nlogicfun>	m_fun;				// Key:protocol id value:handle
		tnotfindfun								m_notfindfun;		// If m_fun findtohandle, handle
	protected:
		void register_logic(i32_protocolnum aprotocol, int32_t aready, const tlogicfun& afun);
	public:
		nrfunbase() = default;

		// # Setprotocol handling
		nrfunbase& set_notfindfun(const tnotfindfun& afun);

		// # Protocol
		void notfindfun(const actor_base* aactor, i32_threadid athreadid, handle_pram& apram)const;

		// # Protocol handling
		bool handle_switch(actor_base* aactor, i32_threadid athreadid, handle_pram& apram);

		template <typename T>
		bool handle_switch(actor_base* aactor, std::shared_ptr<T>& aparm)
		{
			auto lpfun = tools::findmap(m_fun, tprotocol::protocol<T>());
			if (lpfun == nullptr)
			{
				return false;
			}
			handle_pram lpram = handle_pram::create<T, false>(nguid::make(), nguid::make(), aparm);
			lpfun->m_fun(aactor, -1, lpram);
		}
	};

	template <typename T>
	struct message
	{
	private:
		message() = delete;

		std::shared_ptr<T>		m_shared_data		= nullptr;	// Data
		T*						m_original_data		= nullptr;	// Data
		std::shared_ptr<pack>	m_pack				= nullptr;	// Ifmessage, this datapack
		i32_threadid			m_thread			= 0;		// Threadid
	public:
		inline message(i32_threadid athread, const std::shared_ptr<pack>& apack, const std::shared_ptr<T>& adata) :
			m_thread(athread),
			m_pack(apack),
			m_shared_data(adata)
		{
		}

		inline message(i32_threadid athread, std::shared_ptr<pack> apack, T* adata) :
			m_thread(athread),
			m_pack(apack),
			m_original_data(adata)
		{
		}

		inline const T* get_data()const
		{
			if (m_shared_data != nullptr)
			{
				return m_shared_data.get();
			}
			return m_original_data;
		}

		inline const std::shared_ptr<T>& get_shared_data()const
		{
			return m_shared_data;
		}

		inline const pack* get_pack()const
		{
			return m_pack.get();
		}

		inline const std::shared_ptr<pack>& get_shared_pack()const
		{
			return m_pack;
		}

		inline i32_threadid thread()const
		{
			return m_thread;
		}
	};

	template <typename TDerived, typename TPRAM>
	using Tfun = bool (TDerived::*)(const message<TPRAM>&);

	template <typename TDerived>
	class nrfun : public nrfunbase
	{
		nrfun(const nrfun&) = delete;
		nrfun& operator=(const nrfun&) = delete;

		nrfun() = default;
	public:
		static nrfun<TDerived>& instance()
		{
			static nrfun<TDerived> ltemp;
			return ltemp;
		}

		// # Std::function<void(TTTDerived*, T&)> tospecifiedactoron
		template <typename TTTDerived, typename T>
		nrfun& rfun(const std::function<void(TTTDerived*, message<T>&)>& afun, int32_t aready = e_ready_all);

		//# bool aisload = false 
		// # Whether dbdataloadcomplete beforehandlethis message
		template <typename TTTDerived, typename T>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, int32_t aready = e_ready_all);

		// # Actor messagehandle
		template <typename TTTDerived, typename T>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, int32_t aready = e_ready_all);

		// # Actor messagehandle, registernetwork layer
		template <typename TTTDerived, typename T>
		nrfun& rfun_nonet(const Tfun<TTTDerived, T> afun, int32_t aready = e_ready_all);

		// # Gatewayregisterc2g forwardingprotocol handlingprotocol
		template <typename T>
		nrfun& rfun_c2g(const Tfun<TDerived, np_actor_forward<T, forward_c2g<forward>>> afun);

		// # Gatewayregisterg2c forwardingprotocol handlingprotocol
		template <typename T>
		nrfun& rfun_g2c(const Tfun<TDerived, np_actor_forward<T, forward_g2c<forward>>> afun);		
	};
}//namespace ngl