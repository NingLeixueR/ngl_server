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

	class schedule_layer
	{
	private:
		schedule_layer(const schedule_layer&) = delete;
		schedule_layer& operator=(const schedule_layer&) = delete;

		std::deque<ptrnthread>		m_workthreads;		// Idle workers available for scheduling.
		std::deque<ptrnthread>		m_workthreadscopy;	// Full worker set kept for bookkeeping/debugging.
		bool						m_suspend = false;	// Freeze dispatch while world-state critical sections run.
		std::deque<ptrnthread>		m_suspendthreads;	// Workers parked during suspension.
		i32_threadsize				m_threadnum = -1;	// Configured worker count.
		std::unordered_map<nguid, ptractor, guid_hash> m_actorbyid;		// Fast lookup by full actor guid.
		std::map<nguid, ptractor>	m_actorbroadcast;	// Actors that receive periodic broadcast ticks.
		std::deque<ptractor>		m_actorlist;		// Actors waiting for a worker.
		std::set<i16_actortype>		m_actortype;		// Distinct actor types currently registered.
		std::map<nguid, std::function<void()>>			m_delactorfun;	// Deferred callbacks to run after in-flight actors finish.
		std::map<ENUM_ACTOR, std::map<i64_actorid, ptractor>> m_actorbytype;	// Lookup by actor type, then guid.

		std::shared_mutex			m_mutex;
		ngl::tools::sem				m_sem;
		std::jthread				m_thread;			// Start last so the dispatcher sees initialized state.

		bool is_unavailable(ngl::actor_stat astat) noexcept;

		// Resolve a local actor or fall back to the route actor for forwarded traffic.
		ptractor* nosafe_get_actorbyid(const nguid& aguid);

		// Push work into an actor queue and schedule it if the actor was idle.
		void nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram);

		// Dispatcher loop: pair queued actors with available workers.
		void run(std::stop_token astop);
	public:

		schedule_layer();

		~schedule_layer() noexcept;

		// Return the actor id used as the node-level routing endpoint.
		static nguid get_clientguid();

		// Resolve the routing actor for this node type.
		static nguid nodetypebyguid();

		// Create the worker pool once during startup.
		void init(i32_threadsize apthreadnum, int32_t alayer);

		// Return the set of currently registered actor types.
		void get_type(std::set<i16_actortype>& aactortype);

		// Register a newly created actor instance.
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		// Remove an actor and optionally run a callback after release.
		void erase_actor(const nguid& aguid, const std::function<void()>& afun = nullptr);

		// Return whether the actor is still registered.
		bool is_have_actor(const nguid& aguid);

		// Return a worker to the pool and reschedule the actor if more work remains.
		void push(const ptractor& apactor, ptrnthread atorthread = nullptr, bool aready = true);

		// Route work to one actor or a set of actors.
		bool push_task_id(const nguid& aguid, handle_pram& apram, bool aismass = true);
		bool push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		// Broadcast work to all actors of a given type.
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

		// Deliver the periodic broadcast task to actors that opted in.
		void broadcast_task(handle_pram& apram);

		// Pause and resume dispatch while critical shared state is updated.
		// Suspension intentionally leaves one worker outside the parked pool.
		void statrt_suspend_thread();
		void finish_suspend_thread();

		// Return the number of registered actors.
		int32_t actor_count();

		// Collect scheduler statistics grouped by actor type.
		void get_actor_stat(msg_actor_stat& adata);
	};

	class actor_manage
	{
		static constexpr int32_t LAYER_COUNT = 4;

		actor_manage(const actor_manage&) = delete;
		actor_manage& operator=(const actor_manage&) = delete;
		actor_manage() = default;
		~actor_manage() = default;

		std::array<std::shared_ptr<schedule_layer>, LAYER_COUNT> m_layers;

		int32_t layer_index(int64_t actorid) const noexcept
		{
			return static_cast<int32_t>(static_cast<uint64_t>(actorid) % LAYER_COUNT);
		}

		std::shared_ptr<schedule_layer>& get_layer(int64_t actorid)
		{
			return m_layers[layer_index(actorid)];
		}
	public:
		static actor_manage& instance()
		{
			static actor_manage ltemp;
			return ltemp;
		}

		// Create the worker pool once during startup.
		void init(i32_threadsize apthreadnum);

		// Return the set of currently registered actor types.
		void get_type(std::vector<i16_actortype>& aactortype);

		// Build a handle_pram from a typed payload and enqueue it for the target actor.
		template <typename T, bool IS_SEND = true>
		inline void push_task_id(const nguid& aguid, std::shared_ptr<T>& apram)
		{
			handle_pram lparm = handle_pram::create<T, IS_SEND>(aguid, nguid::make(), apram);
			m_layers[layer_index(aguid)]->push_task_id(aguid, lparm);
		}

		// Register a newly created actor instance.
		bool add_actor(actor_base* apactor, const std::function<void()>& afun);

		// Register a newly created actor instance.
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		// Remove an actor and optionally run a callback after release.
		void erase_actor(const nguid& aguid, const std::function<void()>& afun = nullptr);

		// Return whether the actor is still registered.
		bool is_have_actor(const nguid& aguid);

		// Return a worker to the pool and reschedule the actor if more work remains.
		void push(const ptractor& apactor, ptrnthread atorthread = nullptr, bool aready = true);

		// Route work to one actor or a set of actors.
		void push_task_id(const nguid& aguid, handle_pram& apram);
		void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		// Broadcast work to all actors of a given type.
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

		// Deliver the periodic broadcast task to actors that opted in.
		void broadcast_task(handle_pram& apram);

		// Pause and resume dispatch while critical shared state is updated.
		// Suspension intentionally leaves one worker outside the parked pool.
		void statrt_suspend_thread();
		void finish_suspend_thread();

		// Return the number of registered actors.
		int32_t actor_count();

		// Collect scheduler statistics grouped by actor type.
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
