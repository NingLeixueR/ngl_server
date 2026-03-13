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
#include "actor/actor_logic/actor_client/actor_client.h"
#include "actor/actor_logic/actor_server/actor_server.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/actor_db.h"
#include "tools/tools.h"

#include <iostream>
#include <format>

namespace ngl
{
	actor_manage::actor_manage():
		m_thread([this](std::stop_token astop)
			{
				run(astop);
			})
	{
	}

	actor_manage::~actor_manage()
	{
		m_thread.request_stop();
		m_sem.post();
	}

	void actor_manage::init(i32_threadsize apthreadnum)
	{
		if (!m_workthreads.empty())
		{
			return;
		}
		m_threadnum = apthreadnum;
		for (int32_t i = 0; i < m_threadnum; ++i)
		{
			m_workthreads.push_back(std::make_shared<nthread>(i));
		}
		m_workthreadscopy = m_workthreads;
	}

	void actor_manage::get_type(std::vector<i16_actortype>& aactortype)
	{
		lock_read(m_mutex);
		std::vector<i16_actortype> ltypes;
		ltypes.reserve(m_actortype.size());
		std::copy(m_actortype.begin(), m_actortype.end(), std::back_inserter(ltypes));
		aactortype.swap(ltypes);
	}

	void actor_manage::nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram)
	{
		actor_stat lstat = lpactor->activity_stat();
		if (lstat == actor_stat_close || lstat == actor_stat_init)
		{
			std::cout << std::format("actor_manage push task fail actor:{} stat:{}", lpactor->guid(), static_cast<int32_t>(lstat)) << std::endl;
			return;
		}
		lpactor->push(apram);
		if (lstat == actor_stat_free)
		{
			m_actorlist.push_back(lpactor);
			lpactor->set_activity_stat(actor_stat_list);
			if (!m_suspend && !m_workthreads.empty())
			{
				m_sem.post();
			}
		}
	}

	bool actor_manage::add_actor(const ptractor& apactor, const std::function<void()>& afun)
	{
		const nguid& guid = apactor->guid();
		const bool lneedsync = apactor->type() != ACTOR_CLIENT && apactor->type() != ACTOR_SERVER;
		const nguid lrouteactor = nodetypebyguid();
		bool lhasrouteactor = false;
		{
			nlock(m_mutex);
			if (m_actorbyid.contains(guid))
			{
				std::cout << std::format("actor_manage add_actor duplicate guid:{}", guid) << std::endl;
				return false;
			}
			m_actorbyid[guid] = apactor;
			m_actortype.insert(apactor->type());
			m_actorbytype[apactor->type()][guid] = apactor;
			if (apactor->isbroadcast())
			{
				m_actorbroadcast[guid] = apactor;
			}
			lhasrouteactor = lneedsync && m_actorbyid.contains(lrouteactor);
		}
		if (lneedsync)
		{
			// 新增的actor 
			if (lhasrouteactor)
			{
				auto pro = std::make_shared<np_actornode_update_mass>(
				np_actornode_update_mass
				{
					.m_mass = np_actornode_update
					{
						.m_id = nconfig.nodeid(),
						.m_add = {guid},
					},
					.m_fun = afun
				}
			);
				push_task_id<np_actornode_update_mass, false>(lrouteactor, pro);
			}
			else
			{
				std::cout << std::format("actor_manage add_actor skip route sync guid:{} route_actor:{}", guid, lrouteactor) << std::endl;
			}
		}
		else
		{
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
		ptractor ltemp(apactor, [](actor_base*) {});
		return add_actor(ltemp, afun);
	}

	void actor_manage::erase_actor(const nguid& aguid, const std::function<void()>& afun /*= nullptr*/)
	{
		const nguid lrouteactor = nodetypebyguid();
		bool lhasrouteactor = false;
		{
			nlock(m_mutex);
			lhasrouteactor = m_actorbyid.contains(lrouteactor);
		}
		// 通知actor_client已经删除actor 
		if (lhasrouteactor)
		{
			auto pro = std::make_shared<np_actornode_update_mass>(
			np_actornode_update_mass
			{
				.m_mass = np_actornode_update
				{
					.m_id = nconfig.nodeid(),
					.m_del = {aguid}
				}
			}
		);
			push_task_id<np_actornode_update_mass, false>(lrouteactor, pro);
		}

		bool isrunfun = false;
		ptractor lpactor = nullptr;
		{
			nlock(m_mutex);
			const ptractor* lpactorptr = tools::findmap(m_actorbyid, aguid);
			if (lpactorptr == nullptr)
			{
				std::cout << std::format("actor_manage erase_actor missing guid:{}", aguid) << std::endl;
				return;
			}
			lpactor = *lpactorptr;

			// # 从actor_manage中移除
			m_actorbyid.erase(aguid);
			auto type_it = m_actorbytype.find(aguid.type());
			if (type_it != m_actorbytype.end())
			{
				type_it->second.erase(aguid);
				if (type_it->second.empty())
				{
					m_actorbytype.erase(type_it);
					m_actortype.erase(aguid.type());
				}
			}
			m_actorbroadcast.erase(aguid);

			if (lpactor->activity_stat() == actor_stat_list)
			{
				auto litorfind = std::find_if(m_actorlist.begin(), m_actorlist.end(), [&aguid](const ptractor& ap)->bool
					{
						return aguid == ap->id_guid();
					}
				);
				if (litorfind != m_actorlist.end())
				{
					m_actorlist.erase(litorfind);
				}
				lpactor->set_activity_stat(actor_stat_close);
				isrunfun = true;
			}
			else if (lpactor->activity_stat() == actor_stat_free)
			{
				isrunfun = true;
			}
			else
			{
				if (afun != nullptr)
				{
					m_delactorfun.try_emplace(lpactor->id_guid(), afun);
				}
			}
			lpactor->set_activity_stat(actor_stat_close);
		}
		if (isrunfun)
		{
			lpactor->release();
			if (afun != nullptr)
			{
				afun();
			}
		}
	}

	bool actor_manage::is_have_actor(const nguid& aguid)
	{
		nlock(m_mutex);
		return m_actorbyid.contains(aguid);
	}

	void actor_manage::push(const ptractor& apactor, ptrnthread atorthread/* = nullptr*/)
	{
		std::function<void()> lfun = nullptr;
		bool lrelease = false;
		bool lneedpost = false;
		{
			nlock(m_mutex);
			if (atorthread != nullptr)
			{
				if (m_suspend)
				{
					m_suspendthreads.push_back(atorthread);
				}
				else
				{
					m_workthreads.push_back(atorthread);
				}
			}
			if (!m_actorbyid.contains(apactor->id_guid()))
			{//erase_actor
				nguid leraseguid = apactor->id_guid();
				std::function<void()>* lpfun = tools::findmap(m_delactorfun, leraseguid);
				if (lpfun != nullptr)
				{
					lfun.swap(*lpfun);
					m_delactorfun.erase(leraseguid);
					apactor->set_activity_stat(actor_stat_close);
					lrelease = true;
				}
			}
			else
			{
				if(!apactor->list_empty())
				{
					m_actorlist.push_back(apactor);
					apactor->set_activity_stat(actor_stat_list);
				}
				else
				{
					apactor->set_activity_stat(actor_stat_free);
				}
			}
			lneedpost = !m_suspend && !m_actorlist.empty() && !m_workthreads.empty();
		}
		if (lneedpost)
		{
			m_sem.post();
		}
		if (lrelease)
		{
			apactor->release();
			lfun();
		}
	}

	ptractor& actor_manage::nosafe_get_actor(const nguid& aguid)
	{
		static ptractor lnullptr(nullptr);
		auto lpactor = tools::findmap(m_actorbyid, aguid);
		if (lpactor == nullptr)
		{
			return lnullptr;
		}
		return *lpactor;
	}

	ptractor& actor_manage::nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram)
	{
		static ptractor lnull(nullptr);
		ptractor* lpactor = tools::findmap(m_actorbyid, aguid);
		if (lpactor == nullptr)
		{
			if (!apram.m_issend)
			{
				return lnull;
			}
			// 发给actor_client/actor_server
			// 如果是actor_server结点需要发送给actor_server
			nguid lguid = nodetypebyguid();
			lpactor = tools::findmap(m_actorbyid, lguid);
			if (lpactor == nullptr)
			{
				return lnull;
			}
		}
		return *lpactor;
	}

	void actor_manage::push_task_id(const nguid& aguid, handle_pram& apram)
	{
		nlock(m_mutex);
		ptractor lpactor = nosafe_get_actorbyid(aguid, apram);
		if (lpactor == nullptr || lpactor->activity_stat() == actor_stat_close)
		{
			std::cout << std::format("actor_manage push_task_id fail actor:{}", aguid) << std::endl;
			return;
		}
		nosafe_push_task_id(lpactor, apram);
	}

	void actor_manage::push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram)
	{
		nlock(m_mutex);
		bool lmass = false;
		ptractor lpclient = nullptr;
		const nguid lnodetypeguid = nodetypebyguid();
		for (i64_actorid actorid : asetguid)
		{
			ptractor lpactor = nosafe_get_actorbyid(actorid, apram);
			if (lpactor == nullptr || lpactor->activity_stat() == actor_stat_close)
			{
				continue;
			}
			if (lpactor->id_guid() == lnodetypeguid)
			{
				lmass = true;
				lpclient = lpactor;
				continue;
			}

			handle_pram llocal = handle_pram::shallow_copy_without_massactors(apram);
			nosafe_push_task_id(lpactor, llocal);
		}
		if (lmass && lpclient != nullptr)
		{
			nosafe_push_task_id(lpclient, apram);
		}
	}

	void actor_manage::push_task_type(ENUM_ACTOR atype, handle_pram& apram)
	{
		nlock(m_mutex);
		// 1.先发给本机上的atype
		auto type_it = m_actorbytype.find(atype);
		if (type_it != m_actorbytype.end())
		{
			for (auto& [_guid, _actor] : type_it->second)
			{
				if (_actor->activity_stat() != actor_stat_close)
				{
					nosafe_push_task_id(_actor, apram);
				}
			}
		}
		if (apram.m_issend)
		{
			// 2.然后发给actor_client，发给其他服务器
			nguid lguid = nodetypebyguid();
			ptractor* lpactor = tools::findmap(m_actorbyid, lguid);
			if (lpactor == nullptr)
			{
				return;
			}
			nosafe_push_task_id(*lpactor, apram);
		}
	}

	void actor_manage::broadcast_task(handle_pram& apram)
	{
		nlock(m_mutex);
		for (auto& [_guid, _actor] : m_actorbroadcast)
		{
			if (_actor->isbroadcast())
			{
				nosafe_push_task_id(_actor, apram);
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
		nlock(m_mutex);
		return (int32_t)m_actorbyid.size();
	}

	void actor_manage::get_actor_stat(msg_actor_stat& adata)
	{
		nlock(m_mutex);
		std::vector<msg_actor> lactors;
		lactors.reserve(m_actorbytype.size());
		for (auto& [_type, _map] : m_actorbytype)
		{
			if (_map.empty())
			{
				continue;
			}
			msg_actor ltemp;
			ltemp.m_actor_name = em<ENUM_ACTOR>::name(_type);
			for (auto& [_guid, _actor] : _map)
			{
				ltemp.m_actor[nguid::area(_guid)].push_back(nguid::actordataid(_guid));
			}
			lactors.push_back(std::move(ltemp));
		}
		adata.m_vec = std::move(lactors);
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
		ptrnthread lpthread = nullptr;
		ptractor lpactor = nullptr;

		while (!astop.stop_requested())
		{
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
						lpthread = m_workthreads.front();
						lpactor = m_actorlist.front();
						m_actorlist.pop_front();
						m_workthreads.pop_front();
						lpactor->set_activity_stat(actor_stat_run);
					}
					lpthread->push(lpactor);
				}
			}
		}				
	}
}//namespace ngl
