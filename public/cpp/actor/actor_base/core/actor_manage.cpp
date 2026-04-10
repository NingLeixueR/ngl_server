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
	namespace actor_manage_detail
	{
		bool actor_is_unavailable(ngl::actor_stat astat) noexcept
		{
			// init/close states cannot accept new work and should never be queued.
			return astat == ngl::actor_stat_close || astat == ngl::actor_stat_init;
		}
	} // namespace actor_manage_detail

	actor_manage::actor_manage():
		m_thread([this](std::stop_token astop)
			{
				run(astop);
			})
	{
	}

	actor_manage::~actor_manage() noexcept
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

	void actor_manage::init(i32_threadsize apthreadnum)
	{
		if (!m_workthreads.empty())
		{
			return;
		}
		m_threadnum = apthreadnum > 0 ? apthreadnum : 1;
		for (int32_t i = 0; i < m_threadnum; ++i)
		{
			// Workers start idle and are handed actors only by the dispatcher thread.
			m_workthreads.emplace_back(std::make_shared<nthread>(i));
		}
		m_workthreadscopy = m_workthreads;
	}

	void actor_manage::get_type(std::vector<i16_actortype>& aactortype)
	{
		lock_read(m_mutex);
		std::vector<i16_actortype> ltypes(m_actortype.begin(), m_actortype.end());
		aactortype.swap(ltypes);
	}

	void actor_manage::nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram)
	{
		const actor_stat lstat = lpactor->activity_stat();
		if (actor_manage_detail::actor_is_unavailable(lstat))
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

	bool actor_manage::add_actor(const ptractor& apactor, const std::function<void()>& afun)
	{
		const nguid actor_guid = apactor->guid();
		const ENUM_ACTOR actor_type = apactor->type();
		// actor_client and actor_server are the routing actors themselves, so they do not
		// announce their existence through the route actor.
		const bool needs_route_sync = actor_type != ACTOR_CLIENT && actor_type != ACTOR_SERVER;
		const nguid route_actor_guid = nodetypebyguid();
		bool has_route_actor = false;
		{
			nlock(m_mutex);
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
			has_route_actor = needs_route_sync && m_actorbyid.contains(route_actor_guid);
		}
		if (needs_route_sync)
		{
			// Notify the route actor so other nodes can discover the new actor location.
			if (has_route_actor)
			{
				auto pro = std::make_shared<np_actornode_update_mass>(np_actornode_update_mass{
						.m_mass = np_actornode_update
						{
							.m_id = nconfig.nodeid(),
							.m_add = {actor_guid},
						},
						.m_fun = afun
					}
				);
				push_task_id<np_actornode_update_mass, false>(route_actor_guid, pro);
			}
			else
			{
				std::cout << std::format("actor_manage add_actor skip route sync guid:{} route_actor:{}", actor_guid, route_actor_guid) << std::endl;
			}
		}
		else
		{
			// Route actors become immediately available because nothing needs to sync them first.
			apactor->set_activity_stat(actor_stat_free);
		}
		return true;
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
		return add_actor(actor_ref, afun);
	}

	void actor_manage::erase_actor(const nguid& aguid, const std::function<void()>& afun /*= nullptr*/)
	{
		const nguid route_actor_guid = nodetypebyguid();
		bool has_route_actor = false;
		{
			nlock(m_mutex);
			has_route_actor =m_actorbyid.contains(route_actor_guid);
		}
		// Inform the route actor first so remote nodes stop targeting this actor.
		if (has_route_actor)
		{
			auto pro = std::make_shared<np_actornode_update_mass>(np_actornode_update_mass{
					.m_mass = np_actornode_update
					{
						.m_id = nconfig.nodeid(),
						.m_del = {aguid}
					}
				}
			);
			push_task_id<np_actornode_update_mass, false>(route_actor_guid, pro);
		}

		bool should_run_callback = false;
		ptractor actor_ref = nullptr;
		{
			nlock(m_mutex);
			// Remove the actor from all manager-side lookup tables before scheduling teardown.
			if(!tools::erasemap(m_actorbyid, aguid, actor_ref))
			{
				std::cout << std::format("actor_manage erase_actor missing guid:{}", aguid) << std::endl;
				return;
			}
			const ENUM_ACTOR actor_type = actor_ref->type();
			auto type_it = m_actorbytype.find(actor_type);
			if (type_it != m_actorbytype.end())
			{
				type_it->second.erase(aguid);
				if (type_it->second.empty())
				{
					m_actorbytype.erase(type_it);
					m_actortype.erase(static_cast<i16_actortype>(actor_type));
				}
			}
			m_actorbroadcast.erase(aguid);

			if (actor_ref->activity_stat() == actor_stat_list)
			{
				// If the actor is waiting in the scheduler queue, remove it immediately.
				std::erase_if(m_actorlist, [&aguid](const auto& actor) {
					return aguid == actor->id_guid();
				});
				actor_ref->set_activity_stat(actor_stat_close);
				should_run_callback = true;
			}
			else if (actor_ref->activity_stat() == actor_stat_free)
			{
				should_run_callback = true;
			}
			else
			{
				// The actor is currently running, so delay the callback until the worker returns it.
				if (afun != nullptr)
				{
					m_delactorfun.try_emplace(actor_ref->id_guid(), afun);
				}
			}
			actor_ref->set_activity_stat(actor_stat_close);
		}
		if (should_run_callback)
		{
			actor_ref->release();
			if (afun != nullptr)
			{
				afun();
			}
		}
	}

	bool actor_manage::is_have_actor(const nguid& aguid)
	{
		lock_read(m_mutex);
		return m_actorbyid.contains(aguid);
	}

	void actor_manage::push(const ptractor& apactor, ptrnthread atorthread/* = nullptr*/, bool aready/* = true*/)
	{
		std::function<void()> deferred_release_callback = nullptr;
		bool should_release_actor = false;
		bool should_wake_dispatcher = false;
		const nguid actor_guid = apactor->id_guid();
		{
			nlock(m_mutex);
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
			if (!m_actorbyid.contains(actor_guid))
			{//erase_actor
				auto deferred_callback = tools::findmap(m_delactorfun, actor_guid);
				if (deferred_callback != nullptr)
				{
					deferred_release_callback.swap(*deferred_callback);
					m_delactorfun.erase(actor_guid);
					apactor->set_activity_stat(actor_stat_close);
					should_release_actor = true;
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
			should_wake_dispatcher = !m_suspend && !m_actorlist.empty() && !m_workthreads.empty();
		}
		if (should_wake_dispatcher)
		{
			m_sem.post();
		}
		if (should_release_actor)
		{
			apactor->release();
			deferred_release_callback();
		}
	}

	ptractor* actor_manage::nosafe_get_actor(const nguid& aguid)
	{
		return tools::findmap(m_actorbyid, aguid);
	}

	ptractor* actor_manage::nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram)
	{
		ptractor* actor_ref = tools::findmap(m_actorbyid, aguid);
		if (actor_ref == nullptr)
		{
			if (!apram.m_issend)
			{
				return nullptr;
			}
			// Forwarded messages fall back to the node-level routing actor when the target
			// actor does not exist on this process.
			const nguid route_actor_guid = nodetypebyguid();
			actor_ref = tools::findmap(m_actorbyid, route_actor_guid);
			if (actor_ref == nullptr)
			{
				return nullptr;
			}
		}
		return actor_ref;
	}

	void actor_manage::push_task_id(const nguid& aguid, handle_pram& apram)
	{
		nlock(m_mutex);
		// Lookup is done under the scheduler lock so actor removal and enqueue stay atomic.
		auto lpactor = nosafe_get_actorbyid(aguid, apram);
		if (lpactor == nullptr || actor_manage_detail::actor_is_unavailable((*lpactor)->activity_stat()))
		{
			std::cout << std::format("actor_manage push_task_id fail actor:{}", aguid) << std::endl;
			return;
		}
		nosafe_push_task_id(*lpactor, apram);
	}

	void actor_manage::push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram)
	{
		nlock(m_mutex);
		bool lmass = false;
		ptractor lpclient = nullptr;
		const nguid lnodetypeguid = nodetypebyguid();
		for (i64_actorid actorid : asetguid)
		{
			auto lpactor = nosafe_get_actorbyid(actorid, apram);
			if (lpactor == nullptr || actor_manage_detail::actor_is_unavailable((*lpactor)->activity_stat()))
			{
				continue;
			}
			if ((*lpactor)->id_guid() == lnodetypeguid)
			{
				// Preserve the original mass-send payload only for the route actor.
				lmass = true;
				lpclient = *lpactor;
				continue;
			}

			// Local actors should not see the remote mass-send metadata.
			handle_pram llocal = handle_pram::shallow_copy_without_massactors(apram);
			nosafe_push_task_id(*lpactor, llocal);
		}
		if (lmass && lpclient != nullptr)
		{
			nosafe_push_task_id(lpclient, apram);
		}
	}

	void actor_manage::push_task_type(ENUM_ACTOR atype, handle_pram& apram)
	{
		nlock(m_mutex);
		// 1. Deliver locally to every actor of the requested type on this node.
		auto type_it = m_actorbytype.find(atype);
		if (type_it != m_actorbytype.end())
		{
			for (const auto& lpair : type_it->second)
			{
				const ptractor& actor_ref = lpair.second;
				if (!actor_manage_detail::actor_is_unavailable(actor_ref->activity_stat()))
				{
					nosafe_push_task_id(actor_ref, apram);
				}
			}
		}
		if (apram.m_issend)
		{
			// 2. Forward the same message to the route actor so remote nodes can receive it too.
			const nguid route_actor_guid = nodetypebyguid();
			ptractor* route_actor = tools::findmap(m_actorbyid, route_actor_guid);
			if (route_actor == nullptr)
			{
				return;
			}
			nosafe_push_task_id(*route_actor, apram);
		}
	}

	void actor_manage::broadcast_task(handle_pram& apram)
	{
		nlock(m_mutex);
		for (const auto& lpair : m_actorbroadcast)
		{
			const ptractor& actor_ref = lpair.second;
			// Broadcast ticks are opt-in and still respect the actor lifecycle state.
			if (actor_ref->isbroadcast() && !actor_manage_detail::actor_is_unavailable(actor_ref->activity_stat()))
			{
				nosafe_push_task_id(actor_ref, apram);
			}
		}
	}

	void actor_manage::statrt_suspend_thread()
	{
		int lthreadnum = 0;
		while (lthreadnum + 1 < m_threadnum)
		{
			{
				nlock(m_mutex);
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

	void actor_manage::finish_suspend_thread()
	{
		nlock(m_mutex);
		m_suspend = false;
		m_workthreads.insert(m_workthreads.end(), m_suspendthreads.begin(), m_suspendthreads.end());
		m_suspendthreads.clear();
		m_sem.post();
	}

	int32_t actor_manage::actor_count()
	{
		lock_read(m_mutex);
		return (int32_t)m_actorbyid.size();
	}

	void actor_manage::get_actor_stat(msg_actor_stat& adata)
	{
		nlock(m_mutex);
		std::vector<msg_actor> actor_stats;
		actor_stats.reserve(m_actorbytype.size());
		for (auto& [actor_type, actors_by_guid] : m_actorbytype)
		{
			if (actors_by_guid.empty())
			{
				continue;
			}
			msg_actor actor_stat_entry;
			actor_stat_entry.m_actor_name = tools::em<ENUM_ACTOR>::name(actor_type);
			for (auto& [actor_guid, _] : actors_by_guid)
			{
				actor_stat_entry.m_actor[nguid::area(actor_guid)].push_back(nguid::actordataid(actor_guid));
			}
			actor_stats.emplace_back(std::move(actor_stat_entry));
		}
		adata.m_vec = std::move(actor_stats);
	}

	nguid actor_manage::nodetypebyguid()
	{
		return nconfig.nodetype() == ACTORSERVER ? actor_server::actorid() : actor_client::actorid();
	}

	nguid actor_manage::get_clientguid()
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

	void actor_manage::run(std::stop_token astop)
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
			{
				for (;;)
				{
					{
						nlock(m_mutex);
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
	}
}//namespace ngl
