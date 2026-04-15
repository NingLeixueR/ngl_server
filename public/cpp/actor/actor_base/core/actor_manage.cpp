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
// File overview: Implements logic for actor base.

#include "actor/actor_logic/actor_client/actor_client.h"
#include "actor/actor_logic/actor_server/actor_server.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/actor_db.h"
#include "tools/tools.h"

#include <iostream>
#include <format>

namespace ngl
{
	schedule_layer::schedule_layer():
		m_thread([this](std::stop_token astop)
			{
				run(astop);
			})
	{
	}

	schedule_layer::~schedule_layer() noexcept
	{
		try
		{
			m_thread.request_stop();
			m_sem.post();

			if (m_thread.joinable())
			{
				m_thread.join();
			}

			std::deque<ptrnthread> lworkers;
			{
				nlock(m_mutex);
				lworkers = m_workthreadscopy;
			}

			for (const ptrnthread& lworker : lworkers)
			{
				if (lworker != nullptr)
				{
					lworker->shutdown();
				}
			}

			nlock(m_mutex);
			m_workthreads.clear();
			m_workthreadscopy.clear();
			m_suspendthreads.clear();
			m_actorlist.clear();
			m_actorbroadcast.clear();
			m_actorbyid.clear();
			m_delactorfun.clear();
			m_actorbytype.clear();
			m_actortype.clear();
		}
		catch (...)
		{
			// Process teardown must not fail because actor cleanup raised an exception.
		}
	}

	bool schedule_layer::is_unavailable(ngl::actor_stat astat) noexcept
	{
		// init/close states cannot accept new work and should never be queued.
		return astat == ngl::actor_stat_close || astat == ngl::actor_stat_init;
	}

	void schedule_layer::init(i32_threadsize apthreadnum, int32_t alayer)
	{
		if (!m_workthreads.empty())
		{
			return;
		}
		m_threadnum = apthreadnum > 0 ? apthreadnum : 1;
		for (int32_t i = 0; i < m_threadnum; ++i)
		{
			// Workers start idle and are handed actors only by the dispatcher thread.
			m_workthreads.emplace_back(std::make_shared<nthread>(alayer* apthreadnum + i));
		}
		m_workthreadscopy = m_workthreads;
	}

	void schedule_layer::get_type(std::set<i16_actortype>& aactortype)
	{
		lock_read(m_mutex);
		aactortype.insert(m_actortype.begin(), m_actortype.end());
	}

	void schedule_layer::nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram)
	{
		const actor_stat lstat = lpactor->activity_stat();
		if (is_unavailable(lstat))
		{
			std::cout << std::format("actor_manage push task fail actor:{} stat:{}", lpactor->guid(), static_cast<int32_t>(lstat)) << std::endl;
			return;
		}
		if (lpactor->push(apram) && lstat == actor_stat_free)
		{
			// Only transition free actors into the ready queue. Running/list actors will be
			// rescheduled by the worker thread once they finish the current batch.
			m_actorlist.emplace_back(lpactor);
			lpactor->set_activity_stat(actor_stat_list);
			if (!m_suspend && !m_workthreads.empty())
			{
				m_sem.post();
			}
		}
	}

	bool schedule_layer::add_actor(const ptractor& apactor, const std::function<void()>& afun)
	{
		const nguid actor_guid = apactor->guid();
		const ENUM_ACTOR actor_type = apactor->type();
		// actor_client and actor_server are the routing actors themselves, so they do not
		// announce their existence through the route actor.
		const bool needs_route_sync = actor_type != ACTOR_CLIENT && actor_type != ACTOR_SERVER;
		const nguid route_actor_guid = nodetypebyguid();
		{
			lock_write(m_mutex);
			if (!m_actorbyid.try_emplace(actor_guid, apactor).second)
			{
				std::cout << std::format("actor_manage add_actor duplicate guid:{}", actor_guid) << std::endl;
				return false;
			} 
			m_actortype.insert(static_cast<i16_actortype>(actor_type));
			m_actorbytype[actor_type].try_emplace(actor_guid, apactor);
			if (apactor->isbroadcast())
			{
				m_actorbroadcast.try_emplace(actor_guid, apactor);
			}
		}
		if (needs_route_sync)
		{
			// Notify the route actor so other nodes can discover the new actor location.
			auto pro = std::make_shared<np_actornode_update_mass>(np_actornode_update_mass{
						.m_mass = np_actornode_update
						{
							.m_id = nconfig.nodeid(),
							.m_add = {actor_guid},
						},
						.m_fun = afun
				}
			);
			actor_manage::instance().push_task_id<np_actornode_update_mass, false>(route_actor_guid, pro);
		}
		else
		{
			// Route actors become immediately available because nothing needs to sync them first.
			apactor->set_activity_stat(actor_stat_free);
		}
		return true;
	}

	void schedule_layer::erase_actor(const nguid& aguid, const std::function<void()>& afun /*= nullptr*/)
	{
		const nguid route_actor_guid = nodetypebyguid();
		auto pro = std::make_shared<np_actornode_update_mass>(np_actornode_update_mass{
					.m_mass = np_actornode_update
					{
						.m_id = nconfig.nodeid(),
						.m_del = {aguid}
					}
			}
		);
		actor_manage::instance().push_task_id<np_actornode_update_mass, false>(route_actor_guid, pro);

		bool should_run_callback = false;
		ptractor lpactor = nullptr;
		{
			lock_write(m_mutex);
			// Remove the actor from all manager-side lookup tables before scheduling teardown.
			if(!tools::erasemap(m_actorbyid, aguid, lpactor))
			{
				std::cout << std::format("actor_manage erase_actor missing guid:{}", aguid) << std::endl;
				return;
			}
			const ENUM_ACTOR ltype = lpactor->type();
			auto type_it = m_actorbytype.find(ltype);
			if (type_it != m_actorbytype.end())
			{
				type_it->second.erase(aguid);
				if (type_it->second.empty())
				{
					m_actorbytype.erase(type_it);
					m_actortype.erase(static_cast<i16_actortype>(ltype));
				}
			}
			m_actorbroadcast.erase(aguid);

			if (lpactor->activity_stat() == actor_stat_list)
			{
				// If the actor is waiting in the scheduler queue, remove it immediately.
				std::erase_if(m_actorlist, [&aguid](const auto& actor) {
					return aguid == actor->id_guid();
				});
				lpactor->set_activity_stat(actor_stat_close);
				should_run_callback = true;
			}
			else if (lpactor->activity_stat() == actor_stat_free)
			{
				should_run_callback = true;
			}
			else
			{
				// The actor is currently running, so delay the callback until the worker returns it.
				if (afun != nullptr)
				{
					m_delactorfun.try_emplace(lpactor->id_guid(), afun);
				}
			}
			lpactor->set_activity_stat(actor_stat_close);
		}
		if (should_run_callback)
		{
			lpactor->release();
			if (afun != nullptr)
			{
				afun();
			}
		}
	}

	bool schedule_layer::is_have_actor(const nguid& aguid)
	{
		lock_read(m_mutex);
		return m_actorbyid.contains(aguid);
	}

	void schedule_layer::push(const ptractor& apactor, ptrnthread atorthread/* = nullptr*/, bool aready/* = true*/)
	{
		std::function<void()> ldefcallback = nullptr;
		bool lshould_release_actor = false;
		bool lshould_wake_dispatcher = false;
		const nguid lguid = apactor->id_guid();
		{
			lock_write(m_mutex);
			if (atorthread != nullptr)
			{
				// Workers return themselves here after finishing an actor batch.
				if (m_suspend)
				{
					m_suspendthreads.emplace_back(atorthread);
				}
				else
				{
					m_workthreads.emplace_back(atorthread);
				}
			}
			if (!m_actorbyid.contains(lguid))
			{//erase_actor
				auto lcallback = tools::findmap(m_delactorfun, lguid);
				if (lcallback != nullptr)
				{
					ldefcallback.swap(*lcallback);
					m_delactorfun.erase(lguid);
					apactor->set_activity_stat(actor_stat_close);
					lshould_release_actor = true;
				}
			}
			else
			{
				const bool lhigh = !apactor->high_empty();
				if (!apactor->list_empty() && (aready || lhigh))
				{
					// More messages arrived while the actor was running, so queue it again.
					m_actorlist.emplace_back(apactor);
					apactor->set_activity_stat(actor_stat_list);
				}
				else
				{
					// No pending work left; the actor goes back to the idle pool.
					apactor->set_activity_stat(actor_stat_free);
				}
			}
			lshould_wake_dispatcher = !m_suspend && !m_actorlist.empty() && !m_workthreads.empty();
		}
		if (lshould_wake_dispatcher)
		{
			m_sem.post();
		}
		if (lshould_release_actor)
		{
			apactor->release();
			ldefcallback();
		}
	}

	ptractor* schedule_layer::nosafe_get_actorbyid(const nguid& aguid)
	{
		return tools::findmap(m_actorbyid, aguid);
	}

	bool schedule_layer::push_task_id(const nguid& aguid, handle_pram& apram, bool aismass/* = true*/)
	{
		do
		{
			lock_write(m_mutex);
			// Lookup is done under the scheduler lock so actor removal and enqueue stay atomic.
			auto lpactor = nosafe_get_actorbyid(aguid);
			if (lpactor == nullptr)
			{
				if (!apram.m_issend)
				{
					std::cout << std::format("actor_manage push_task_id fail actor:{}", aguid) << std::endl;
					return false;
				}
				break;
			}
			if (is_unavailable((*lpactor)->activity_stat()))
			{
				std::cout << std::format("actor_manage push_task_id fail actor:{}", aguid) << std::endl;
				return false;
			}
			nosafe_push_task_id(*lpactor, apram);
			return false;
		} while (false);		
		
		// Forwarded messages fall back to the node-level routing actor when the target
		// actor does not exist on this process.
		if (aismass)
		{
			actor_manage::instance().push_task_id(nodetypebyguid(), apram);
		}

		return true;
	}

	bool schedule_layer::push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram)
	{
		bool lmass = false;
		for (i64_actorid actorid : asetguid)
		{
			lmass = lmass || push_task_id(actorid, apram, false);
		}
		return lmass;		
	}

	void schedule_layer::push_task_type(ENUM_ACTOR atype, handle_pram& apram)
	{
		{
			lock_write(m_mutex);
			// 1. Deliver locally to every actor of the requested type on this node.
			auto litor = m_actorbytype.find(atype);
			if (litor != m_actorbytype.end())
			{
				for (const auto& lpair : litor->second)
				{
					const ptractor& lpactors = lpair.second;
					if (!is_unavailable(lpactors->activity_stat()))
					{
						nosafe_push_task_id(lpactors, apram);
					}
				}
			}
		}
	}

	void schedule_layer::broadcast_task(handle_pram& apram)
	{
		lock_write(m_mutex);
		for (const auto& lpair : m_actorbroadcast)
		{
			const ptractor& actor_ref = lpair.second;
			// Broadcast ticks are opt-in and still respect the actor lifecycle state.
			if (actor_ref->isbroadcast() && !is_unavailable(actor_ref->activity_stat()))
			{
				nosafe_push_task_id(actor_ref, apram);
			}
		}
	}

	void schedule_layer::statrt_suspend_thread()
	{
		int lthreadnum = 0;
		while (lthreadnum + 1 < m_threadnum)
		{
			{
				lock_write(m_mutex);
				// Keep one thread outside the suspended pool so the caller can continue making progress.
				m_suspend = true;
				if (!m_workthreads.empty())
				{
					m_suspendthreads.insert(m_suspendthreads.end(), m_workthreads.begin(), m_workthreads.end());
					m_workthreads.clear();
				}
				lthreadnum = (int)m_suspendthreads.size();
			}

			if (lthreadnum + 1 < m_threadnum)
			{
				std::this_thread::yield();
			}
		}
	}

	void schedule_layer::finish_suspend_thread()
	{
		lock_write(m_mutex);
		m_suspend = false;
		m_workthreads.insert(m_workthreads.end(), m_suspendthreads.begin(), m_suspendthreads.end());
		m_suspendthreads.clear();
		m_sem.post();
	}

	int32_t schedule_layer::actor_count()
	{
		lock_read(m_mutex);
		return (int32_t)m_actorbyid.size();
	}

	void schedule_layer::get_actor_stat(std::map<ENUM_ACTOR, msg_actor>& astatmap)
	{
		lock_read(m_mutex);
		for (auto& [actor_type, actors_by_guid] : m_actorbytype)
		{
			if (actors_by_guid.empty())
			{
				continue;
			}
			msg_actor& lstat = astatmap[actor_type];
			lstat.m_actor_name = tools::em<ENUM_ACTOR>::name(actor_type);
			for (auto& [actor_guid, _] : actors_by_guid)
			{
				lstat.m_actor[nguid::area(actor_guid)].push_back(nguid::actordataid(actor_guid));
			}
		}
	}

	nguid schedule_layer::nodetypebyguid()
	{
		return nconfig.nodetype() == ACTORSERVER ? actor_server::actorid() : actor_client::actorid();
	}

	nguid schedule_layer::get_clientguid()
	{
		return nodetypebyguid();
	}

	actor_suspend::actor_suspend()
	{
		log_info()->print("start actor_suspend");
		actor_manage::instance().statrt_suspend_thread();
	}

	actor_suspend::~actor_suspend()
	{		
		Try
		{
			actor_manage::instance().finish_suspend_thread();
			log_info()->print("finish actor_suspend");
		}
		Catch
	}

	void schedule_layer::run(std::stop_token astop)
	{
		ptrnthread worker_thread = nullptr;
		ptractor ready_actor = nullptr;

		while (!astop.stop_requested())
		{
			// The semaphore is posted whenever there may be at least one ready actor and one idle worker.
			m_sem.wait();
			if (astop.stop_requested())
			{
				break;
			}
			for (;;)
			{
				{
					lock_write(m_mutex);
					if (astop.stop_requested() || m_actorlist.empty() || m_workthreads.empty() || m_suspend)
					{
						break;
					}
					// The scheduler is single-threaded: it pops one ready actor and one idle
					// worker, then lets the worker run independently.
					worker_thread = m_workthreads.front();
					ready_actor = m_actorlist.front();
					m_actorlist.pop_front();
					m_workthreads.pop_front();
					ready_actor->set_activity_stat(actor_stat_run);
				}
				worker_thread->push(ready_actor);
			}
		}				
	}

	void actor_manage::init(i32_threadsize apthreadnum)
	{
		int32_t lpthreads = apthreadnum / LAYER_COUNT;
		if (lpthreads <= 0)
		{
			lpthreads = 1;
		}
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			m_layers[i] = std::make_shared<schedule_layer>();
			m_layers[i]->init(lpthreads, i);
		}
	}

	void actor_manage::get_type(std::vector<i16_actortype>& aactortype)
	{
		std::set<i16_actortype> lset;
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			std::set<i16_actortype> lsettemp;
			m_layers[i]->get_type(lsettemp);
			lset.merge(lsettemp);
		}
		aactortype.clear();
		aactortype.insert(aactortype.begin(), lset.begin(), lset.end());
	}

	bool actor_manage::add_actor(actor_base* apactor, const std::function<void()>& afun)
	{
		if (apactor == nullptr)
		{
			return false;
		}
		ptractor actor_ref(apactor, [](actor_base*)
			{
				// The raw pointer is only aliased long enough to forward into the shared_ptr overload.
			});
		return get_layer(apactor->id_guid())->add_actor(actor_ref, afun);
	}

	bool actor_manage::add_actor(const ptractor& apactor, const std::function<void()>& afun)
	{
		return get_layer(apactor->id_guid())->add_actor(apactor, afun);
	}

	void actor_manage::erase_actor(const nguid& aguid, const std::function<void()>& afun /*= nullptr*/)
	{
		get_layer(aguid.id())->erase_actor(aguid, afun);
	}

	bool actor_manage::is_have_actor(const nguid& aguid)
	{
		return get_layer(aguid.id())->is_have_actor(aguid);
	}

	void actor_manage::push(const ptractor& apactor, ptrnthread atorthread /*= nullptr*/, bool aready /*= true*/)
	{
		get_layer(apactor->id_guid())->push(apactor, atorthread, aready);
	}

	void actor_manage::push_task_id(const nguid& aguid, handle_pram& apram)
	{
		get_layer(aguid.id())->push_task_id(aguid, apram);
	}

	void actor_manage::push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram)
	{
		std::array<std::set<i64_actorid>, LAYER_COUNT> larray;
		for (i64_actorid actorid : asetguid)
		{
			larray[layer_index(actorid)].insert(actorid);
		}

		bool lmass = false;
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			if (!larray[i].empty())
			{
				lmass = lmass || m_layers[i]->push_task_id(larray[i], apram);
			}
		}
		if (lmass)
		{
			push_task_id(schedule_layer::nodetypebyguid(), apram);
		}
	}

	void actor_manage::push_task_type(ENUM_ACTOR atype, handle_pram& apram)
	{
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			m_layers[i]->push_task_type(atype, apram);
		}
		if (apram.m_issend)
		{
			actor_manage::instance().push_task_id(schedule_layer::nodetypebyguid(), apram);
		}
	}

	void actor_manage::broadcast_task(handle_pram& apram)
	{
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			m_layers[i]->broadcast_task(apram);
		}
	}

	void actor_manage::statrt_suspend_thread()
	{
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			m_layers[i]->statrt_suspend_thread();
		}
	}
	void actor_manage::finish_suspend_thread()
	{
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			m_layers[i]->finish_suspend_thread();
		}
	}

	int32_t actor_manage::actor_count()
	{
		int32_t lsun = 0;
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			lsun += m_layers[i]->actor_count();
		}
		return lsun;
	}

	void actor_manage::get_actor_stat(msg_actor_stat& adata)
	{
		std::map<ENUM_ACTOR, msg_actor> lstatmap;
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			m_layers[i]->get_actor_stat(lstatmap);
		}
		std::vector<msg_actor> lstats;
		lstats.reserve(lstatmap.size());
		for (auto& [ltype, lstat] : lstatmap)
		{
			lstats.emplace_back(std::move(lstat));
		}
		adata.m_vec = std::move(lstats);
	}
}//namespace ngl
