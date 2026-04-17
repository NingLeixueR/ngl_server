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
// File overview: Layered actor scheduling.
//
// Architecture:
//   schedule_layer  — independent scheduling unit (own mutex, dispatcher thread,
//                     actor tables). Multiple instances run in parallel.
//   actor_manage    — singleton routing layer that fans out to schedule_layer
//                     instances and owns the shared worker pool.
//
// Layer assignment:
//   Singleton actors (type >= ACTOR_SIGNLE_START): type & (LAYER_COUNT-1)
//   Dynamic   actors (ACTOR_ROLE, ACTOR_ROBOT…):   dataid & (LAYER_COUNT-1)
//   This ensures dynamic actors of the same type are spread across layers
//   rather than all landing on one (which would happen with raw id % N,
//   because the type field occupies the lowest bits of the packed nguid).
//
// Worker pool:
//   Workers live in actor_manage and are shared across all layers.
//   A dispatcher acquires a worker via pop_free_hreads(), pairs it with a
//   ready actor, and the worker returns itself via push_workthreads() when done.
//
// Lifetime:
//   Both actor_manage and schedule_layer are designed to live for the entire
//   process lifetime. Their threads are detached (not joinable) and their
//   destructors perform no cleanup. This is intentional: adding graceful
//   shutdown would require stop-token plumbing, drain logic, and ordering
//   constraints across layers and workers — complexity that buys nothing
//   when the OS reclaims all resources at process exit anyway.

#pragma once

#include "actor/actor_logic/actor_client/actor_client.h"
#include "actor/actor_base/core/actor_base.h"
#include "actor/actor_base/core/naddress.h"
#include "actor/actor_base/core/nthread.h"
#include "tools/tools/tools_thread.h"
#include "tools/serialize/ndefine.h"
#include "tools/serialize/pack.h"

#include <unordered_map>
#include <atomic>
#include <deque>
#include <map>
#include <set>

namespace ngl
{
	struct guid_hash
	{
		std::size_t operator()(const nguid& aguid) const noexcept
		{
			return std::hash<i64_actorid>{}(aguid.id());
		}
	};
	using ptrnthread = std::shared_ptr<nthread>;

	// One independent scheduling unit. Owns a dispatcher thread and all actor
	// lookup tables for the actors assigned to this layer.
	// Workers are borrowed from the shared pool in actor_manage.
	// All public methods are thread-safe (guarded by m_mutex).
	//
	// Lifetime: constructed once, never destroyed. The dispatcher thread is
	// detached at construction and runs for the process lifetime. No graceful
	// shutdown is implemented — see file-level comment for rationale.
	class schedule_layer
	{
	private:
		schedule_layer(const schedule_layer&) = delete;
		schedule_layer& operator=(const schedule_layer&) = delete;

		std::unordered_map<nguid, ptractor, guid_hash>	m_actorbyid;		// guid -> actor.
		std::map<nguid, ptractor>						m_actorbroadcast;	// Actors that opted into broadcast ticks.
		std::deque<ptractor>							m_actorlist;		// Ready queue: actors waiting for a worker.
		std::set<i16_actortype>							m_actortype;		// Distinct actor types in this layer.
		std::map<nguid, std::function<void()>>			m_delactorfun;		// Deferred erase callbacks for in-flight actors.
		std::map<ENUM_ACTOR, std::map<i64_actorid, ptractor>> m_actorbytype;	// type -> (guid -> actor).

		std::shared_mutex			m_mutex;			// Guards all mutable state above.
		ngl::tools::sem				m_sem;				// Wakes the dispatcher when work is available.
		std::thread					m_thread;			// Detached dispatcher thread owned for process lifetime.

		// Return true if the actor state cannot accept new work.
		bool is_unavailable(ngl::actor_stat astat) noexcept;

		// Look up an actor in this layer only. Returns nullptr if not found.
		ptractor* nosafe_get_actorbyid(const nguid& aguid);

		// Enqueue a message and transition the actor to the ready queue if idle.
		// Caller must hold m_mutex.
		void nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram);

		// Dispatcher loop: drains the ready queue by pairing actors with workers.
		// Runs forever on a detached thread — no stop condition by design.
		void run();
	public:

		schedule_layer();

		// Destructor is trivial — the detached dispatcher thread is abandoned at
		// process exit. No drain or join logic needed (see file-level comment).
		~schedule_layer() noexcept = default;

		// Return the guid of the node-level routing actor (actor_client or actor_server).
		static nguid get_clientguid();
		static nguid nodetypebyguid();

		// Append this layer's actor types into aactortype.
		void get_type(std::set<i16_actortype>& aactortype);

		// Register an actor in this layer and notify the route actor (via actor_manage).
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		// Remove an actor, notify the route actor, and run afun when safe.
		void erase_actor(const nguid& aguid, const std::function<void()>& afun = nullptr);

		// Check whether an actor is registered in this layer.
		bool is_have_actor(const nguid& aguid);

		// Return a finished worker to the pool and reschedule the actor if it has pending work.
		// Called by nthread after processing an actor batch.
		void push(const ptractor& apactor, bool aready = true);

		// Push a message to a specific actor in this layer.
		// Returns false if delivered locally, true if the caller should forward to the route actor.
		// When aismass is true and the actor is missing, forwards to the route actor directly.
		bool push_task_id(const nguid& aguid, handle_pram& apram, bool aismass = true);

		// Push a message to each actor in asetguid that belongs to this layer.
		// Returns true if any actor was missing and needs route-actor forwarding.
		bool push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		// Deliver a message to every actor of the given type in this layer.
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

		// Deliver a broadcast tick to all opted-in actors in this layer.
		void broadcast_task(handle_pram& apram);

		// Return the number of actors registered in this layer.
		int32_t actor_count();

		// Merge this layer's per-type statistics into astatmap.
		void get_actor_stat(std::map<ENUM_ACTOR, msg_actor>& astatmap);
	};

	// Singleton routing layer that owns the shared worker pool and routes every
	// call to the correct schedule_layer. Handles cross-layer concerns:
	// route-actor fallback, statistics merging, and coordinated suspension.
	// External callers use this class exclusively; schedule_layer is internal.
	//
	// Lifetime: heap-allocated singleton, never destroyed. See file-level comment.
	class actor_manage
	{
		actor_manage(const actor_manage&) = delete;
		actor_manage& operator=(const actor_manage&) = delete;

		static constexpr int32_t LAYER_COUNT = 8;	// Must be power of 2 for bit-mask routing.
		static_assert((LAYER_COUNT& (LAYER_COUNT - 1)) == 0, "LAYER_COUNT must be power of 2");

		std::array<std::shared_ptr<schedule_layer>, LAYER_COUNT> m_layers;
		int32_t				m_threadcount = 0;
		struct nwork
		{
			ptrnthread	m_work = nullptr;
			bool		m_free = true;

			nwork()
			{}

			nwork(int32_t aindex) :
				m_work(std::make_shared<nthread>(aindex))
			{}

		};
		std::vector<nwork>			m_workthreads;		// Shared worker pool across all layers.
		std::shared_mutex			m_mutex;			// Guards m_workthreads and m_suspend.
		ngl::tools::sem				m_sem;				// Token count tracks free workers.
		bool						m_suspend = false;	// True while dispatch is frozen.
		// Trivial constructor/destructor — singleton is heap-allocated and never freed.
		actor_manage() = default;
		~actor_manage() = default;


		// Map an actor id to a layer index in [0, LAYER_COUNT).
		// Singleton actors are routed by type (sequential enum values cycle evenly).
		// Dynamic actors are routed by dataid (e.g. player id) so that actors of
		// the same type spread across layers instead of clustering on one.
		int32_t layer_index(int64_t actorid) const noexcept;

		std::shared_ptr<schedule_layer>& get_layer(int64_t actorid);

		// Find a free worker index. Returns -1 if none available or suspended.
		// Caller must hold m_mutex.
		int32_t free_threads();
	public:
		static actor_manage& instance()
		{
			static actor_manage ltemp;
			return ltemp;
		}

		// Block until a free worker is available, then mark it busy and return it.
		// Called by schedule_layer dispatchers from their detached threads.
		ptrnthread pop_free_hreads();

		// Return a worker to the shared pool after it finishes processing an actor.
		void push_workthreads(ptrnthread atorthread);

		// Create apthreadnum workers (shared pool) and LAYER_COUNT dispatcher layers.
		void init(i32_threadsize apthreadnum);

		// Collect the union of actor types across all layers.
		void get_type(std::vector<i16_actortype>& aactortype);

		// Convenience: build a handle_pram from a typed payload and route it to the target layer.
		template <typename T, bool IS_SEND = true>
		inline void push_task_id(const nguid& aguid, std::shared_ptr<T>& apram)
		{
			handle_pram lparm = handle_pram::create<T, IS_SEND>(aguid, nguid::make(), apram);
			m_layers[layer_index(aguid)]->push_task_id(aguid, lparm);
		}

		// Register an actor (raw-pointer overload wraps into shared_ptr).
		bool add_actor(actor_base* apactor, const std::function<void()>& afun);

		// Register an actor in the layer determined by its guid.
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		// Remove an actor and run afun when safe.
		void erase_actor(const nguid& aguid, const std::function<void()>& afun = nullptr);

		// Check whether an actor exists in any layer.
		bool is_have_actor(const nguid& aguid);

		// Return a finished worker and actor to the correct layer.
		void push(const ptractor& apactor, ptrnthread atorthread = nullptr, bool aready = true);

		// Route a message to one actor, falling back to the route actor if missing.
		void push_task_id(const nguid& aguid, handle_pram& apram);

		// Partition a set of targets by layer, deliver locally, and forward misses to the route actor.
		void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		// Deliver to all actors of a type across all layers, then forward to the route actor if needed.
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

		// Deliver a broadcast tick to all opted-in actors across all layers.
		void broadcast_task(handle_pram& apram);

		// Suspend / resume all dispatch. statrt_suspend_thread() spin-waits until
		// every worker is idle; finish_suspend_thread() releases them.
		void statrt_suspend_thread();
		void finish_suspend_thread();

		// Return the total number of actors across all layers.
		int32_t actor_count();

		// Collect and merge per-type statistics from all layers.
		void get_actor_stat(msg_actor_stat& adata);
	};


	// RAII helper that pauses actor dispatch for the lifetime of the object.
	class actor_suspend
	{
		actor_suspend(const actor_suspend&) = delete;
		actor_suspend& operator=(const actor_suspend&) = delete;
	public:
		actor_suspend();
		~actor_suspend();
	};

	// Lazily create singleton actors, then register their protocol handlers once.
	template <typename T>
	T& actor_instance<T>::instance()
	{
		static T ltemp;
		static std::once_flag lfirst;
		std::call_once(lfirst, []()
			{
				T* lptemp = &ltemp;
				actor_manage::instance().add_actor(&ltemp, [lptemp]()
					{
						lptemp->set_activity_stat(actor_stat_free);
						lptemp->init();
						lptemp->init_db_component(false);
					}
				);
				ltemp.template init_rfun<T>();
				T::nregister();
			}
		);
		return ltemp;
	}
}//namespace ngl

namespace ngl
{
	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(ENUM_ACTOR atype, const std::shared_ptr<T>& adata)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(nguid::make_self(atype), nguid::make(), adata);
		lpram.m_forwardtype = true;
		actor_manage::instance().push_task_type(atype, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata);
		actor_manage::instance().push_task_id(aguid, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata, const std::function<void()>& afailfun)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata, afailfun);
		actor_manage::instance().push_task_id(aguid, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const std::set<i64_actorid>& asetguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
	{
		if (!asetguid.empty())
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(asetguid, arequestguid, adata);
			actor_manage::instance().push_task_id(asetguid, lpram);
		}
	}

	template <typename T>
	void actor_base::send_client(const std::set<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
		pro->m_data.m_protocol = aprotocol;
		pro->m_data.m_data = adata;
		pro->m_data.m_uid.reserve(aids.size());
		pro->m_data.m_area.reserve(aids.size());
		for (i64_actorid aactorid : aids)
		{
			nguid lguid(aactorid);
			pro->m_data.m_uid.emplace_back(lguid.actordataid());
			pro->m_data.m_area.emplace_back(lguid.area());
		}
		handle_pram lpram = handle_pram::create<T>(nguid::make(), nguid::make(), pro);
		actor_manage::instance().push_task_id(actorclient_guid(), lpram);
	}

	template <typename T>
	void actor_base::send_client(i64_actorid aid, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		std::set<i64_actorid> lids = { aid };
		send_client(lids, adata, aprotocol);
	}

	template <typename T>
	void actor_base::send_client(const std::vector<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		std::set<i64_actorid> lids(aids.cbegin(), aids.cend());
		send_client(lids, adata, aprotocol);
	}

	template <typename T>
	void actor_base::send_client(const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		const std::set<i32_serverid>& lgatewayids = sysconfig::gatewayids();
		if (!lgatewayids.empty())
		{
			auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
			pro->m_data.m_protocol = aprotocol;
			pro->m_data.m_data = adata;
			nguid lguid(nguid::make());
			pro->m_data.m_uid.emplace_back(lguid.actordataid());
			pro->m_data.m_area.emplace_back(lguid.area());
			send_server(lgatewayids, *pro, nguid::make(), nguid::make());
		}
	}

	template <typename T>
	void actor_base::send_clientbyarea(i16_area aarea, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
		pro->m_data.m_protocol = aprotocol;
		pro->m_data.m_data = adata;
		ttab_servers::instance().foreach_server(GATEWAY, aarea, [&pro](const tab_servers* atab)
			{
				send_server(atab->m_id, *pro, nguid::make(), nguid::make());
			}
		);
	}
}//namespace ngl
