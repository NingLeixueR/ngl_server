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
#include "actor_client.h"
#include "actor_server.h"
#include "actor_manage.h"
#include "actor_db.h"
#include "tools.h"

namespace ngl
{
	actor_manage::~actor_manage()
	{
	}

	actor_manage::actor_manage():
		m_thread(&actor_manage::run, this)
	{
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
			m_workthreads.push_back(new nthread(i));
		}
	}

	void actor_manage::get_type(std::vector<i16_actortype>& aactortype)
	{
		aactortype.reserve(m_actortype.size());
		std::copy(m_actortype.begin(), m_actortype.end(), std::back_inserter(aactortype));
	}

	void actor_manage::nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram)
	{
		actor_stat lstat = lpactor->get_activity_stat();
		if (lstat == actor_stat_close || lstat == actor_stat_init)
		{
			std::cout << std::format("actor_mange push task actor:{} stat:{} fail", lpactor->guid(), (int32_t)lstat) << std::endl;
			return;
		}
		lpactor->push(apram);
		if (lpactor->get_activity_stat() == actor_stat_free)
		{
			m_actorlist.push_back(lpactor);
			lpactor->set_activity_stat(actor_stat_list);
		}
		ngl_post;
	}

	bool actor_manage::add_actor(const ptractor& apactor, const std::function<void()>& afun)
	{
		const nguid& guid = apactor->guid();
		{
			ngl_lock_s;
			if (m_actorbyid.contains(guid))
			{
				std::cout << std::format("actor_manage add_actor m_actorbyid.contains(guid:{}) fail", guid) << std::endl;
				return false;
			}
			m_actorbyid[guid] = apactor;
			m_actortype.insert(apactor->type());
			m_actorbytype[apactor->type()][guid] = apactor;
			if (apactor->isbroadcast())
			{
				m_actorbroadcast[guid] = apactor;
			}
		}
		if (apactor->type() != ACTOR_CLIENT && apactor->type() != ACTOR_SERVER)
		{
			// 新增的actor 
			auto pro = std::make_shared<np_actornode_update_mass>(
				np_actornode_update_mass
				{
					.m_mass = np_actornode_update
					{
						.m_id = nconfig::m_nodeid,
						.m_add = {guid},
					},
					.m_fun = afun
				}
			);
			push_task_id<np_actornode_update_mass, false>(nodetypebyguid(), pro);
		}
		else
		{
			apactor->set_activity_stat(actor_stat_free);
		}
		return true;
	}

	bool actor_manage::add_actor(actor_base* apactor, const std::function<void()>& afun)
	{
		ptractor ltemp(apactor);
		return add_actor(ltemp, afun);
	}

	void actor_manage::erase_actor(const nguid& aguid, const std::function<void()>& afun /*= nullptr*/)
	{
		// 通知actor_client已经删除actor 
		auto pro = std::make_shared<np_actornode_update_mass>(
			np_actornode_update_mass
			{
				.m_mass = np_actornode_update
				{
					.m_id = nconfig::m_nodeid,
					.m_del = {aguid}
				}
			}
		);
		push_task_id<np_actornode_update_mass, false>(nodetypebyguid(), pro);

		bool isrunfun = false;
		ptractor lpactor = nullptr;
		{
			ngl_lock_s;
			const ptractor* lpactorptr = tools::findmap(m_actorbyid, aguid);
			if (lpactorptr == nullptr)
			{
				std::cout << std::format("actor_manage erase_actor_byid m_actorbyid.find(guid:{}) fail", aguid) << std::endl;
				return;
			}
			lpactor = *lpactorptr;

			// # 从actor_manage中移除
			m_actorbyid.erase(aguid);
			m_actorbytype[aguid.type()].erase(aguid);
			m_actorbroadcast.erase(aguid);

			if (lpactor->get_activity_stat() == actor_stat_list)
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
			else if (lpactor->get_activity_stat() == actor_stat_free)
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
			afun();
		}
	}

	bool actor_manage::is_have_actor(const nguid& aguid)
	{
		ngl_lock_s;
		return m_actorbyid.contains(aguid);
	}

	void actor_manage::push(const ptractor& apactor, nthread* atorthread/* = nullptr*/)
	{
		std::function<void()> lfun = nullptr;
		bool lrelease = false;
		{
			ngl_lock_s;
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
					ngl_post;
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
				ngl_post;
			}
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
		auto itor = m_actorbyid.find(aguid);
		if (itor == m_actorbyid.end())
		{
			return lnullptr;
		}
		return itor->second;
	}

	ptractor& actor_manage::nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram)
	{
		static ptractor lnull(nullptr);
		ptractor* lpactorptr = tools::findmap(m_actorbyid, aguid);
		if (lpactorptr == nullptr)
		{
			if (!apram.m_issend)
			{
				return lnull;
			}
			// 发给actor_client/actor_server
			// 如果是actor_server结点需要发送给actor_server
			nguid lguid = nodetypebyguid();
			lpactorptr = tools::findmap(m_actorbyid, lguid);
			if (lpactorptr == nullptr)
			{
				return lnull;
			}
		}
		return *lpactorptr;
	}

	void actor_manage::push_task_id(const nguid& aguid, handle_pram& apram)
	{
		ngl_lock_s;
		ptractor lpptractor = nosafe_get_actorbyid(aguid, apram);
		if (lpptractor == nullptr || lpptractor->get_activity_stat() == actor_stat_close)
		{
			std::cout << "push_task_id fail !!!" << std::endl;
			return;
		}
		nosafe_push_task_id(lpptractor, apram);
	}

	void actor_manage::push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram)
	{
		ngl_lock_s;
		bool lmass = false;
		ptractor lpclient = nullptr;
		for (i64_actorid actorid : asetguid)
		{
			ptractor lpptractor = nosafe_get_actorbyid(actorid, apram);
			if (lpptractor == nullptr || lpptractor->get_activity_stat() == actor_stat_close)
			{
				continue;
			}
			if (lpptractor->id_guid() == nodetypebyguid())
			{
				lmass = true;
				lpclient = lpptractor;
				continue;
			}
			nosafe_push_task_id(lpptractor, apram);
		}
		if (lmass)
		{
			nosafe_push_task_id(lpclient, apram);
		}
	}

	void actor_manage::push_task_type(ENUM_ACTOR atype, handle_pram& apram)
	{
		ngl_lock_s;
		// 1.先发给本机上的atype
		for (const auto& [key, value] : m_actorbytype[atype])
		{
			if (value->get_activity_stat() != actor_stat_close)
			{
				nosafe_push_task_id(value, apram);
			}
		}
		// 2.然后发给actor_client，发给其他服务器
		nguid lguid = nodetypebyguid();
		ptractor* lpptractor = tools::findmap(m_actorbyid, lguid);
		if (lpptractor == nullptr)
		{
			return;
		}
		nosafe_push_task_id(*lpptractor, apram);
		ngl_post;
	}

	void actor_manage::broadcast_task(handle_pram& apram)
	{
		ngl_lock_s;
		for (const auto& [key, value] : m_actorbroadcast)
		{
			if (value->isbroadcast())
			{
				nosafe_push_task_id(value, apram);
			}
		}
		ngl_post;
	}

	void actor_manage::statrt_suspend_thread()
	{
		int lthreadnum = 0;
		while (lthreadnum + 1 < m_threadnum)
		{
			ngl_lock_s;
			m_suspend = true;
			if (m_workthreads.empty() == false)
			{
				m_suspendthreads.insert(m_suspendthreads.end(), m_workthreads.begin(), m_workthreads.end());
				m_workthreads.clear();
			}
			lthreadnum = (int)m_suspendthreads.size();
		}
	}

	void actor_manage::finish_suspend_thread()
	{
		ngl_lock_s;
		m_suspend = false;
		m_workthreads.insert(m_workthreads.end(), m_suspendthreads.begin(), m_suspendthreads.end());
		m_suspendthreads.clear();
		ngl_post;
	}

	int32_t actor_manage::actor_count()
	{
		ngl_lock_s;
		return (int32_t)m_actorbyid.size();
	}

	void actor_manage::get_actor_stat(msg_actor_stat& adata)
	{
		ngl_lock_s;
		for (const auto& apair : m_actorbytype)
		{
			msg_actor ltemp;
			ltemp.m_actor_name = em<ENUM_ACTOR>::get_name(apair.first);
			for (const auto& aguidprt : apair.second)
			{
				ltemp.m_actor[nguid::area(aguidprt.first)].push_back(nguid::actordataid(aguidprt.first));
			}
			adata.m_vec.push_back(ltemp);
		}
	}

	bool actor_manage::ready(const nguid& aguid)
	{
		ngl_lock_s;
		return m_ready.contains(aguid);
	}

	void actor_manage::set_ready(const nguid& aguid)
	{
		ngl_lock_s;
		m_ready.insert(aguid);
	}

	nguid actor_manage::nodetypebyguid()
	{
		return xmlnode::m_nodetype == ACTORSERVER ? actor_server::actorid() : actor_client::actorid();
	}

	nguid actor_manage::get_clientguid()
	{
		return nodetypebyguid();
	}

	actor_suspendthread::actor_suspendthread()
	{
		log_info()->print("start actor_suspendthread");
		actor_manage::instance().statrt_suspend_thread();
	}

	actor_suspendthread::~actor_suspendthread()
	{		
		Try
		{
			actor_manage::instance().finish_suspend_thread();
			log_info()->print("finish actor_suspendthread");
		}
		Catch
	}

	void actor_manage::run()
	{
		nthread* lpthread = nullptr;
		ptractor lpactor = nullptr;

#ifdef OPEN_SEM
		while (true)
		{
			m_sem.wait();
			{
				for (;;)
				{
					{
						ngl_lock_s;
						if (m_actorlist.empty() || m_workthreads.empty() || m_suspend)
						{
							break;
						}
						lpthread = *m_workthreads.begin();
						lpactor = *m_actorlist.begin();
						m_actorlist.pop_front();
						m_workthreads.pop_front();
						lpactor->set_activity_stat(actor_stat_run);
					}
					lpthread->push(lpactor);
				}
			}
		}
#else
		auto lfun = [this]()
			{
				bool ret = !m_actorlist.empty() && !m_workthread.empty() && !m_suspend;
				return ret;
			};
		while (true)
		{
			cv_lock(m_cv, m_mutex, lfun)
				do
				{
					lpthread = *m_workthread.begin();
					lpactor = *m_actorlist.begin();
					m_actorlist.pop_front();
					m_workthread.pop_front();
					lpactor->set_activity_stat(actor_stat_run);
					lpthread->push(lpactor);
				} while (lfun());
		}
#endif//OPEN_SEM				
	}
}//namespace ngl