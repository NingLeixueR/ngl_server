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
// File overview: Layered actor scheduling implementation.
//
// schedule_layer: self-contained scheduling unit (dispatcher + workers + actor tables).
// actor_manage:   singleton routing layer that fans out to schedule_layer instances.

#include "actor/actor_logic/actor_client/actor_client.h"
#include "actor/actor_logic/actor_server/actor_server.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/actor_db.h"
#include "tools/tools.h"

#include <iostream>
#include <format>

namespace ngl
{
	// ========================================================================
	// schedule_layer — one independent scheduling unit
	// ========================================================================

	schedule_layer::schedule_layer():
		m_thread([this]()
			{
				run();
			})
	{
		m_thread.detach();
	}

	bool schedule_layer::is_unavailable(ngl::actor_stat astat) noexcept
	{
		// init/close states cannot accept new work and should never be queued.
		return astat == ngl::actor_stat_close || astat == ngl::actor_stat_init;
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
			// Actor was idle — move it to the ready queue so the dispatcher picks it up.
			m_actorlist.emplace_back(lpactor);
			lpactor->set_activity_stat(actor_stat_list);
			m_sem.post();
		}
	}

	bool schedule_layer::add_actor(const ptractor& apactor, const std::function<void()>& afun)
	{
		const nguid lguid = apactor->guid();
		const ENUM_ACTOR ltype = apactor->type();
		{
			lock_write(m_mutex);
			if (!m_actorbyid.try_emplace(lguid, apactor).second)
			{
				std::cout << std::format("actor_manage add_actor duplicate guid:{}", lguid) << std::endl;
				return false;
			}
			m_actortype.insert(static_cast<i16_actortype>(ltype));
			m_actorbytype[ltype].try_emplace(lguid, apactor);
			if (apactor->isbroadcast())
			{
				m_actorbroadcast.try_emplace(lguid, apactor);
			}
		}
		if (ltype != ACTOR_CLIENT && ltype != ACTOR_SERVER)
		{
			// Tell the route actor about the new actor so remote nodes can discover it.
			auto pro = std::make_shared<np_actornode_update_mass>(np_actornode_update_mass{
						.m_mass = np_actornode_update
						{
							.m_id = nconfig.nodeid(),
							.m_add = {lguid},
						},
						.m_fun = afun
				}
			);
			actor_manage::instance().push_task_id<np_actornode_update_mass, false>(nodetypebyguid(), pro);
		}
		else
		{
			// Route actors are immediately available — no sync needed.
			apactor->set_activity_stat(actor_stat_free);
		}
		return true;
	}

	void schedule_layer::erase_actor(const nguid& aguid, const std::function<void()>& afun /*= nullptr*/)
	{
		auto pro = std::make_shared<np_actornode_update_mass>(np_actornode_update_mass{
					.m_mass = np_actornode_update
					{
						.m_id = nconfig.nodeid(),
						.m_del = {aguid}
					}
			}
		);
		actor_manage::instance().push_task_id<np_actornode_update_mass, false>(nodetypebyguid(), pro);

		bool lshould_run_callback = false;
		ptractor lpactor = nullptr;
		{
			lock_write(m_mutex);
			// Remove from all lookup tables before scheduling teardown.
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
				// Waiting in the ready queue — pull it out now.
				std::erase_if(m_actorlist, [&aguid](const auto& actor) {
					return aguid == actor->id_guid();
				});
				lpactor->set_activity_stat(actor_stat_close);
				lshould_run_callback = true;
			}
			else if (lpactor->activity_stat() == actor_stat_free)
			{
				lshould_run_callback = true;
			}
			else
			{
				// Actor is running — defer the callback until the worker returns it.
				if (afun != nullptr)
				{
					m_delactorfun.try_emplace(lpactor->id_guid(), afun);
				}
			}
			lpactor->set_activity_stat(actor_stat_close);
		}
		if (lshould_run_callback)
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

	void schedule_layer::push(const ptractor& apactor, bool aready/* = true*/)
	{
		std::function<void()> ldefcallback = nullptr;
		bool lshould_release_actor = false;
		bool lshould_wake_dispatcher = false;
		const nguid lguid = apactor->id_guid();
		{
			lock_write(m_mutex);
			if (!m_actorbyid.contains(lguid))
			{
				// Actor was erased while running — execute the deferred callback.
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
					// More messages arrived while running — re-queue.
					m_actorlist.emplace_back(apactor);
					apactor->set_activity_stat(actor_stat_list);
				}
				else
				{
					// No pending work — return to idle.
					apactor->set_activity_stat(actor_stat_free);
				}
			}
			lshould_wake_dispatcher = !m_actorlist.empty();
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
		
		// Actor not found locally — forward to the route actor for remote delivery.
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
		lock_write(m_mutex);
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

	void schedule_layer::broadcast_task(handle_pram& apram)
	{
		lock_write(m_mutex);
		for (const auto& lpair : m_actorbroadcast)
		{
			const ptractor& lpactor = lpair.second;
			if (lpactor->isbroadcast() && !is_unavailable(lpactor->activity_stat()))
			{
				nosafe_push_task_id(lpactor, apram);
			}
		}
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

	void schedule_layer::run()
	{
		ptrnthread lpthread = nullptr;
		ptractor lpactor = nullptr;

		for (;;)
		{
			m_sem.wait();
			// Drain the ready queue: acquire a worker first, then pop an actor.
			for (;;)
			{
				lpthread = actor_manage::instance().pop_free_hreads();
				{
					lock_write(m_mutex);
					if (m_actorlist.empty())
					{
						// No actor to dispatch — return the worker to the pool.
						actor_manage::instance().push_workthreads(lpthread);
						break;
					}
					lpactor = m_actorlist.front();
					m_actorlist.pop_front();
					lpactor->set_activity_stat(actor_stat_run);
				}
				lpthread->push(lpactor);
			}
		}
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

	// ========================================================================
	// actor_manage — singleton routing layer
	// ========================================================================

	void actor_manage::init(i32_threadsize apthreadnum)
	{
		m_threadcount = apthreadnum;
		m_workthreads.resize(apthreadnum);
		for (int i = 0; i < m_threadcount; ++i)
		{
			m_workthreads[i].m_free = true;
			m_workthreads[i].m_work = std::make_shared<nthread>(i);
			m_sem.post();
		}
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			m_layers[i] = std::make_shared<schedule_layer>();
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
		if (atorthread != nullptr)
		{
			push_workthreads(atorthread);
		}
		get_layer(apactor->id_guid())->push(apactor, aready);
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
		// Forward once to the route actor so remote nodes receive the type-broadcast.
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

	void actor_manage::statrt_suspend_thread()
	{
		int lthreadnum = 0;
		for (;;)
		{
			{
				lock_write(m_mutex);
				m_suspend = true;
				lthreadnum = 0;
				for (int32_t i = 0; i < m_threadcount; ++i)
				{
					if (m_workthreads[i].m_free)
					{
						++lthreadnum;
					}
				}
			}
			if (lthreadnum < m_threadcount)
			{
				std::this_thread::yield();
			}
			else
			{
				return;
			}
		}
	}

	void actor_manage::finish_suspend_thread()
	{
		lock_write(m_mutex);
		m_suspend = false;
		for (int32_t i = 0; i < m_threadcount; ++i)
		{
			m_sem.post();
		}
	}
}//namespace ngl
