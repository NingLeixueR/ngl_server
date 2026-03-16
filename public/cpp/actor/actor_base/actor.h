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
#include "actor/actor_base/nrfun.h"
#include "tools/template_arg.h"
#include "tools/log/nlog.h"

#include <deque>
#include <queue>

namespace ngl
{
	struct actorparm
	{
		actorparmbase	m_parm;
		int32_t			m_weight	= 0x7fffffff;	// : Getthreadafterhandlemessage
		int32_t			m_timeout	= 0x7fffffff;	// :( Actorhandlemessage this time)
		bool			m_broadcast	= false;		// Whethersupportbroadcast(ifneed toloaddbclient, need tosupportbroadcast)
	};

	template <typename T>
	const T* null = (T*)nullptr;

	class actor : 
		public actor_base
	{
		std::list<handle_pram>						m_list;							// Pending message list
		std::map<int32_t, std::list<handle_pram>>	m_hightlist;					// Pending message list(high priority)
		actor_stat									m_stat = actor_stat_init;		// Actorstate
		std::shared_mutex							m_mutex;						// Lock:[m_list:pending message list]
		int32_t										m_weight = 0;					// Translated comment.
		int32_t										m_timeout = 0;					// :( Actorhandlemessage this time)
		bool										m_release = false;				// Translated comment.
		nrfunbase*									m_actorfun = nullptr;			// Register handler
	public:
		template <typename TDerived>
		void init_rfun()
		{
			m_actorfun = &nrfun<TDerived>::instance();
			if (isbroadcast())
			{
				// # Registerbroadcast handler
				register_actornonet<TDerived, np_actor_broadcast>(
					e_ready_all, (Tfun<actor, np_actor_broadcast>) & actor::handle_broadcast
				);
			}
			// # Registeractor closehandler
			register_actornonet<TDerived, np_actor_close>(
				e_ready_all, (Tfun<actor, np_actor_close>) & actor::handle_close
			);
		}

		template <typename TDerived>
		static nrfun<TDerived>& nrf()
		{
			return nrfun<TDerived>::instance();
		}

		// # Register a timer
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, np_timerparm> afun = &TDerived::timer_handle)
		{
			nrf<TDerived>().template rfun_nonet<TDerived, np_timerparm>(afun, e_ready_all);
		}

		// # Used toregister function correspondingactoron
		template <typename TDerived, typename T>
		static void register_actor_s(int32_t aready, const std::function<void(TDerived*, const message<T>&)>& afun)
		{
			nrf<TDerived>().template rfun<TDerived, T>(afun, aready);
		}

		// # Registeractor function(can handle)
		template <typename TDerived, typename ...ARG>
		static void register_actor(int32_t aready, ARG... afun)
		{
			(nrf<TDerived>().template rfun<TDerived, ARG>(afun, aready), ...);
		}

		// # Andregister_actorsimilar registernetwork layer
		template <typename TDerived, typename T>
		static void register_actornonet(enum_ready aready, const Tfun<TDerived, T> afun)
		{
			nrf<TDerived>().template rfun_nonet<TDerived, T>(afun, aready);
		}

		// # Registeractor handlefunction
		template <typename TDerived, typename ...ARG>
		static void register_handle(enum_ready aready)
		{
			(nrf<TDerived>().template rfun<TDerived, ARG>((Tfun<TDerived, ARG>) & TDerived::handle, aready), ...);
		}

		// # Registeractor handlefunction
		template <typename TDerived, typename ...ARG>
		static void register_script_handle(enum_ready aready)
		{
			(nrf<TDerived>().template rfun<actor, ARG>((Tfun<actor, ARG>) & actor::handle_script<ARG>, aready), ...);
		}

		// # Actor_gateway_c2g register forwarding
		template <typename TDerived, typename ...ARG>
		static void register_forward_c2g()
		{
			(nrf<TDerived>().template rfun_c2g<ARG>((Tfun<TDerived, np_actor_forward<ARG, forward_c2g<forward>>>) & TDerived::handle), ...);
		}
		
		// # Register_forward_g2c register forwarding
		template <typename TDerived, typename ...ARG>
		static void register_forward_g2c()
		{
			(nrf<TDerived>().template rfun_g2c<ARG>((Tfun<TDerived, np_actor_forward<ARG, forward_g2c<forward>>>) & TDerived::handle), ...);
		}

		// # Actor_role registersecondaryforwarding
		template <typename TDerived, ENUM_ACTOR ACTOR, typename ...ARG>
		static void register_secondary_forward_c2g()
		{
			(nrf<TDerived>().template rfun<TDerived, ARG>((Tfun<TDerived, ARG>) & TDerived::template handle_forward<ACTOR, ARG>, e_ready_all), ...);
		}

		explicit actor(const actorparm& aparm);

		// # Get the actor state
		actor_stat activity_stat() final;

		// # Set the actor state
		void set_activity_stat(actor_stat astat) final;

		// # Actormessage list
		void release() final;

		// # Message listwhether
		bool list_empty() final;

		// # Tomessage listinadd message
		void push(handle_pram& apram) final;

		// # Thread message
		void actor_handle(i32_threadid athreadid) final;

		template <typename T>
		bool ahandle(const std::shared_ptr<T>& aparm)
		{
			if (m_actorfun == nullptr)
			{
				tools::no_core_dump();
				return false;
			}
			return m_actorfun->handle_switch(this, aparm);
		}

		bool ahandle(i32_threadid athreadid, handle_pram& aparm);

		// ############# [Broadcast] #############
		// # Time broadcast
		// # 1, Save data
		// # 2, Broadcastfunction
		// # Andactor_base::start_broadcast()
		// # This actor_base::m_broadcast event
		virtual void broadcast() {}
		// # Broadcast handler
		bool handle_broadcast(const message<np_actor_broadcast>& adata);
		// ############# [Broadcast] #############
		
		// # Closethis actor
		bool handle_close(const message<np_actor_close>&);

		// # Script-side message handling
		template <typename TMESSAGE>
		bool handle_script(const message<TMESSAGE>& adata);
	};

	template <typename TMESSAGE>
	bool actor::handle_script(const message<TMESSAGE>& adata)
	{
		const TMESSAGE& ldata = *adata.get_data();
		return nscript_handle(ldata);
	}
}//namespace ngl