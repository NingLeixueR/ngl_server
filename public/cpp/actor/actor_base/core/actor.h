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

#include "actor/actor_base/core/nactortype.h"
#include "actor/actor_base/core/actor_base.h"
#include "tools/tools/tools_template_arg.h"
#include "actor/actor_base/core/nrfun.h"
#include "actor/protocol/tprotocol.h"
#include "tools/log/nlog.h"

#include <deque>
#include <queue>

namespace ngl
{
	struct actorparm
	{
		actorparmbase	m_parm;							// Base identity plus DB/script options owned by actor_base.
		int32_t			m_weight	= 0x7fffffff;	// Maximum number of normal-priority messages per scheduler slice.
		int32_t			m_timeout	= 0x7fffffff;	// Soft time budget for one actor_handle() batch in milliseconds.
		bool			m_broadcast	= false;		// Enable participation in the shared periodic broadcast tick.
	};

	template <typename T>
	const T* null = (T*)nullptr;

	class actor : 
		public actor_base
	{
		std::list<handle_pram>						m_list;							// FIFO queue for regular-priority messages.
		std::map<int32_t, std::list<handle_pram>>	m_hightlist;					// Buckets for priority messages, keyed by protocol priority.
		actor_stat									m_stat = actor_stat_init;		// Scheduler-visible actor state.
		std::shared_mutex							m_mutex;						// Protects both pending-message queues and release state.
		int32_t										m_weight = 0;					// Max number of regular messages handled in one batch.
		int32_t										m_timeout = 0;					// Max wall-clock time for one actor_handle() batch.
		bool										m_release = false;				// Signals that the actor is shutting down and should drain immediately.
		nrfunbase*									m_actorfun = nullptr;			// Type-erased message dispatcher registered by init_rfun().
	public:
		template <typename TDerived>
		void init_rfun()
		{
			m_actorfun = &nrfun<TDerived>::instance();
			if (isbroadcast())
			{
				// Actors that opt into shared broadcasts receive a synthetic maintenance message.
				register_actornonet<TDerived, np_actor_broadcast>(
					(Tfun<actor, np_actor_broadcast>) & actor::handle_broadcast
				);
			}
			// Every actor handles the generic close event through the same local hook.
			register_actornonet<TDerived, np_actor_close>(
				(Tfun<actor, np_actor_close>) & actor::handle_close
			);
		}

		template <typename TDerived>
		static nrfun<TDerived>& nrf()
		{
			return nrfun<TDerived>::instance();
		}

		// Register the timer callback used by ntimer for this actor type.
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, np_timerparm> afun = &TDerived::timer_handle)
		{
			nrf<TDerived>().template rfun_nonet<TDerived, np_timerparm>(afun);
		}

		// Register one std::function-based message handler for this actor type.
		template <typename TDerived, typename T>
		static void register_actor_s(const std::function<void(TDerived*, const message<T>&)>& afun)
		{
			nrf<TDerived>().template rfun<TDerived, T>(afun);
		}

		// Register one or more strongly typed message handlers for this actor type.
		template <typename TDerived, typename ...ARG>
		static void register_actor(ARG... afun)
		{
			(nrf<TDerived>().template rfun<TDerived, ARG>(afun), ...);
		}

		// Register one or more local-only handlers that do not come from network protocol dispatch.
		template <typename TDerived, typename T>
		static void register_actornonet(const Tfun<TDerived, T> afun)
		{
			nrf<TDerived>().template rfun_nonet<TDerived, T>(afun);
		}

		// Register TDerived::handle for every listed message type.
		template <typename TDerived, typename ...ARG>
		static void register_handle()
		{
			(nrf<TDerived>().template rfun<TDerived, ARG>((Tfun<TDerived, ARG>) & TDerived::handle), ...);
		}

		// Register script-backed handlers that forward messages into the actor's scripting runtime.
		template <typename TDerived, typename ...ARG>
		static void register_script_handle()
		{
			(nrf<TDerived>().template rfun<actor, ARG>((Tfun<actor, ARG>) & actor::handle_script<ARG>), ...);
		}

		// Register client-to-gateway forwarding handlers.
		template <typename TDerived, typename ...ARG>
		static void register_forward_c2g()
		{
			(nrf<TDerived>().template rfun_c2g<ARG>((Tfun<TDerived, np_actor_forward<ARG, forward_c2g<forward>>>) & TDerived::handle), ...);
		}
		
		// Register gateway-to-client forwarding handlers.
		template <typename TDerived, typename ...ARG>
		static void register_forward_g2c()
		{
			(nrf<TDerived>().template rfun_g2c<ARG>((Tfun<TDerived, np_actor_forward<ARG, forward_g2c<forward>>>) & TDerived::handle), ...);
		}

		// Register secondary forwarding helpers used by relay actors such as role actors.
		template <typename TDerived, ENUM_ACTOR ACTOR, typename ...ARG>
		static void register_secondary_forward_c2g()
		{
			(nrf<TDerived>().template rfun<TDerived, ARG>((Tfun<TDerived, ARG>) & TDerived::template handle_forward<ACTOR, ARG>), ...);
		}

		explicit actor(const actorparm& aparm);

		// Return the scheduler-visible lifecycle state.
		actor_stat activity_stat() final;

		// Update the scheduler-visible lifecycle state.
		void set_activity_stat(actor_stat astat) final;

		// Stop the actor, drain queued work, and persist any DB-backed state.
		void release() final;

		// Return true when both normal and priority queues are empty.
		bool list_empty() final;

		// Enqueue one incoming task into the appropriate priority queue.
		bool push(handle_pram& apram) final;

		// Run one scheduling slice on the specified worker thread id.
		bool actor_handle(i32_threadid athreadid) final;

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

		// Called from the shared process-wide broadcast event after DB state has been flushed.
		virtual void broadcast() {}
		// Internal entry point for the synthetic broadcast message.
		bool handle_broadcast(const message<np_actor_broadcast>& adata);
		
		// Internal entry point for the synthetic close message.
		bool handle_close(const message<np_actor_close>&);

		// Forward a typed message into the attached scripting runtime.
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
