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
		if (!m_workthread.empty())
		{
			return;
		}
		m_threadnum = apthreadnum;
		for (int32_t i = 0; i < m_threadnum; ++i)
		{
			m_workthread.push_back(new nthread(i));
		}
	}

	void actor_manage::get_type(std::vector<i16_actortype>& aactortype)
	{
		for (i16_actortype item : m_actortype)
		{
			aactortype.push_back(item);
		}
	}

	// # nosafe_��ͷ�ĺ�������"�ڲ�����δ����"���������������
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
			// ������actor 
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
		// ֪ͨactor_client�Ѿ�ɾ��actor 
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
			ptractor* lpactorptr = tools::findmap(m_actorbyid, aguid);
			if (lpactorptr == nullptr)
			{
				std::cout << std::format("actor_manage erase_actor_byid m_actorbyid.find(guid:{}) fail", aguid) << std::endl;
				return;
			}
			lpactor = *lpactorptr;

			// # ��actor_manage���Ƴ�
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
					m_suspendthread.push_back(atorthread);
				}
				else
				{
					m_workthread.push_back(atorthread);
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
				if (!apactor->list_empty())
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
			// ����actor_client/actor_server
			// �����actor_server�����Ҫ���͸�actor_server
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
		// 1.�ȷ��������ϵ�atype
		for (const auto& [key, value] : m_actorbytype[atype])
		{
			if (value->get_activity_stat() != actor_stat_close)
			{
				nosafe_push_task_id(value, apram);
			}
		}
		// 2.Ȼ�󷢸�actor_client����������������
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
			if (m_workthread.empty() == false)
			{
				m_suspendthread.insert(m_suspendthread.end(), m_workthread.begin(), m_workthread.end());
				m_workthread.clear();
			}
			lthreadnum = (int)m_suspendthread.size();
		}
	}

	void actor_manage::finish_suspend_thread()
	{
		ngl_lock_s;
		m_suspend = false;
		m_workthread.insert(m_workthread.end(), m_suspendthread.begin(), m_suspendthread.end());
		m_suspendthread.clear();
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
		Catch;		
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
						if (m_actorlist.empty() || m_workthread.empty() || m_suspend)
						{
							break;
						}
						lpthread = *m_workthread.begin();
						lpactor = *m_actorlist.begin();
						m_actorlist.pop_front();
						m_workthread.pop_front();
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