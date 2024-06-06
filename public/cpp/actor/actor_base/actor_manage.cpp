#include "actor_client.h"
#include "actor_server.h"
#include "actor_manage.h"
#include "actor_db.h"
#include "tools.h"

namespace ngl
{
	struct actor_manage::impl_actor_manage
	{
		//// ---- 线程相关
		std::list<nthread*>	m_workthread;		// 工作线程
		bool				m_suspend;			// 是否挂起
		std::list<nthread*>	m_suspendthread;	// 挂起的工作线程
		std::thread			m_thread;			// 管理线程
		i32_threadsize		m_threadnum;		// 工作线程数量

		ngl_lockinit;

		//// ----- 索引actor
		std::map<nguid, ptractor>							m_actorbyid;
		std::map<ENUM_ACTOR, std::map<nguid, ptractor>>		m_actorbytype;
		std::list<ptractor>									m_actorlist;	// 有任务的actor列表
		std::set<i16_actortype>								m_actortype;	// 包含哪些actortype

		// 删除actor后需要执行的操作
		std::map<nguid, std::function<void()>>				m_delactorfun;

		impl_actor_manage() :
			m_thread(&impl_actor_manage::run, this),
			m_threadnum(0),
			m_suspend(false)
		{}

		inline void init(i32_threadsize apthreadnum)
		{
			if (!m_workthread.empty())
				return;

			m_threadnum = apthreadnum;
			for (int i = 0; i < m_threadnum; ++i)
				m_workthread.push_back(new nthread(i));
		}

		inline void get_type(std::vector<i16_actortype>& aactortype)
		{
			for (i16_actortype item : m_actortype)
				aactortype.push_back(item);
		}

		inline bool add_actor(ptractor& apactor, const std::function<void()>& afun)
		{
			nguid& guid = apactor->guid();
			{
				ngl_lock;
				if (m_actorbyid.find(guid) != m_actorbyid.end())
					return false;
				m_actorbyid[guid] = apactor;
				m_actortype.insert(apactor->type());
				m_actorbytype[apactor->type()][guid] = apactor;
			}

			if (apactor->type() != ACTOR_CLIENT && apactor->type() != ACTOR_SERVER)
			{
				// 新增的actor 
				std::shared_ptr<np_actornode_update_mass> pro(new np_actornode_update_mass
					{
						.m_mass = np_actornode_update
						{
							.m_id = nconfig::m_nodeid,
							.m_add = {guid},
							.m_actorservermass = false
						},
						.m_fun = afun,
					});

				nguid lclientguid = actor_client::actorid();
				handle_pram lparm = handle_pram::create(lclientguid, nguid::make(), pro);
				push_task_id(lclientguid, lparm, false);
			}
			else
			{
				apactor->set_activity_stat(actor_stat_free);
			}
			return true;
		}

		inline void erase_actor_byid(const nguid& aguid, const std::function<void()>& afun)
		{
			bool isrunfun = false;
			std::shared_ptr<np_actornode_update_mass> pro(new np_actornode_update_mass
				{
					.m_mass = np_actornode_update
					{
						.m_id = nconfig::m_nodeid,
						.m_del = {aguid.id()}
					},
				});
			// 删除的actor 
			nguid lclientguid = actor_client::actorid();
			handle_pram lparm = handle_pram::create(lclientguid, nguid::make(), pro);
			push_task_id(lclientguid, lparm, false);

			ptractor lpactor = nullptr;
			{
				ngl_lock;
				auto itor = m_actorbyid.find(aguid);
				if (itor == m_actorbyid.end())
					return;
				lpactor = itor->second;

				m_actorbyid.erase(itor);
				m_actorbytype[aguid.type()].erase(aguid);

				if (lpactor->get_activity_stat() == actor_stat_list)
				{
					auto litorfind = std::find_if(
						m_actorlist.begin(), 
						m_actorlist.end(), 
						[&aguid](ptractor& ap)->bool
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
						m_delactorfun.insert(std::make_pair(lpactor->id_guid(), afun));
				}
				lpactor->set_activity_stat(actor_stat_close);
			}
			if (isrunfun)
			{
				lpactor->release();
				afun();
			}
		}

		inline ptractor& nosafe_get_actor(const nguid& aguid)
		{
			static ptractor lnullptr(nullptr);
			auto itor = m_actorbyid.find(aguid);
			if (itor == m_actorbyid.end())
				return lnullptr;
			return itor->second;
		}

		inline ptractor& get_actor(const nguid& aguid)
		{
			ngl_lock;
			return nosafe_get_actor(aguid);
		}

		inline bool is_have_actor(const nguid& aguid)
		{
			ngl_lock;
			return m_actorbyid.find(aguid) != m_actorbyid.end();
		}

		inline void push(ptractor& apactor, nthread* atorthread)
		{
			std::function<void()> lfun = nullptr;
			while (true)
			{
				ngl_lock;
				if (atorthread != nullptr)
				{
					if (m_suspend)
						m_suspendthread.push_back(atorthread);
					else
						m_workthread.push_back(atorthread);
				}
				auto itor = m_actorbyid.find(apactor->id_guid());
				if (itor == m_actorbyid.end())
				{//erase_actor_byid
					auto itorfun = m_delactorfun.find(apactor->id_guid());
					if (itorfun != m_delactorfun.end())
					{
						lfun.swap(itorfun->second);
						m_delactorfun.erase(itorfun);
						apactor->set_activity_stat(actor_stat_close);

						ngl_post;
						
						break;
					}
				}
				if (apactor->list_empty() == false)
				{
					m_actorlist.push_back(apactor);
					apactor->set_activity_stat(actor_stat_list);
				}
				else
				{
					apactor->set_activity_stat(actor_stat_free);
				}
				ngl_post;
				break;
			}
			if (lfun != nullptr)
			{
				apactor->release();
				lfun();
			}
		}

		inline void nosafe_push_task_id(ptractor& lpactor, handle_pram& apram)
		{
			if (lpactor->get_activity_stat() == actor_stat_close 
				|| lpactor->get_activity_stat() == actor_stat_init)
			{
				std::cout << "activity_stat =  " << (int)lpactor->get_activity_stat() <<" !!!" << std::endl;
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

		inline ptractor& nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram, bool abool)
		{
			static ptractor lnull(nullptr);
			auto itor = m_actorbyid.find(aguid);
			if (itor == m_actorbyid.end())
			{
				if (!abool)
					return lnull;
				//发给actor_client
				nguid lguid = actor_client::actorid();
				itor = m_actorbyid.find(lguid);
				if (itor == m_actorbyid.end())
					return lnull;
			}
			return itor->second;
		}

		inline void push_task_id(const nguid& aguid, handle_pram& apram, bool abool)
		{
			ngl_lock;
			ptractor lpptractor = nosafe_get_actorbyid(aguid, apram, abool);
			if (lpptractor == nullptr || lpptractor->get_activity_stat() == actor_stat_close)
			{
				std::cout << "push_task_id fail !!!" << std::endl;
				return;
			}				
			nosafe_push_task_id(lpptractor, apram);
		}

		inline void push_task_type(ENUM_ACTOR atype, handle_pram& apram, bool aotherserver)
		{
			ngl_lock;
			// 单例不允许群发
			if (enum_actor::is_signle(atype) && aotherserver)
			{
				std::cout << "enum_actor::is_signle("<< atype <<") && "<< aotherserver << std::endl;
				return;
			}
			// 1.先发给本机上的atype
			for (std::pair<const nguid, ptractor>& lpair : m_actorbytype[atype])
			{
				if (lpair.second->get_activity_stat() != actor_stat_close)
					nosafe_push_task_id(lpair.second, apram);
			}
			ngl_post;
			// 2.然后发给actor_client，发给其他服务器
			if (aotherserver == true)
			{
				nguid lguid = actor_client::actorid();
				auto itor = m_actorbyid.find(lguid);
				if (itor == m_actorbyid.end())
					return;
				nosafe_push_task_id(itor->second, apram);
			}
		}

		inline void broadcast_task(handle_pram& apram)
		{
			ngl_lock;
			for (std::pair<const nguid, ptractor>& lpair : m_actorbyid)
			{
				if (lpair.second->isbroadcast())
					nosafe_push_task_id(lpair.second, apram);
			}
			ngl_post;
		}

		inline void statrt_suspend_thread()
		{
			int lthreadnum = 0;
			while (lthreadnum + 1 < m_threadnum)
			{
				ngl_lock;
				m_suspend = true;
				if (m_workthread.empty() == false)
				{
					m_suspendthread.insert(m_suspendthread.end(), m_workthread.begin(), m_workthread.end());
					m_workthread.clear();
				}
				lthreadnum = m_suspendthread.size();
			}
		}

		inline void finish_suspend_thread()
		{
			ngl_lock;
			m_suspend = false;
			m_workthread.insert(m_workthread.end(), m_suspendthread.begin(), m_suspendthread.end());
			m_suspendthread.clear();
			ngl_post;
		}

		inline void run()
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
							monopoly_shared_lock(m_mutex);
							if (m_actorlist.empty() || m_workthread.empty() || m_suspend)
								break;
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
			}
#endif//OPEN_SEM				
		}
	};

	actor_manage::~actor_manage()
	{
	}

	actor_manage::actor_manage()
	{
		m_impl_actor_manage.make_unique();
	}

	void actor_manage::init(i32_threadsize apthreadnum)
	{
		m_impl_actor_manage()->init(apthreadnum);
	}

	void actor_manage::get_type(std::vector<i16_actortype>& aactortype)
	{
		m_impl_actor_manage()->get_type(aactortype);
	}

	bool actor_manage::add_actor(ptractor& apactor, const std::function<void()>& afun)
	{
		return m_impl_actor_manage()->add_actor(apactor, afun);
	}

	bool actor_manage::add_actor(actor_base* apactor, const std::function<void()>& afun)
	{
		ptractor ltemp(apactor);
		return add_actor(ltemp, afun);
	}

	void actor_manage::erase_actor_byid(const nguid& aguid, const std::function<void()>& afun /*= nullptr*/)
	{
		m_impl_actor_manage()->erase_actor_byid(aguid, afun);
	}

	bool actor_manage::is_have_actor(const nguid& aguid)
	{
		return m_impl_actor_manage()->is_have_actor(aguid);
	}

	void actor_manage::push(ptractor& apactor, nthread* atorthread/* = nullptr*/)
	{
		m_impl_actor_manage()->push(apactor, atorthread);
	}

	void actor_manage::push_task_id(const nguid& aguid, handle_pram& apram, bool abool)
	{
		m_impl_actor_manage()->push_task_id(aguid, apram, abool);
	}

	void actor_manage::push_task_type(ENUM_ACTOR atype, handle_pram& apram, bool aotherserver/* = false*/)
	{
		m_impl_actor_manage()->push_task_type(atype, apram, aotherserver);
	}

	void actor_manage::broadcast_task(handle_pram& apram)
	{
		m_impl_actor_manage()->broadcast_task(apram);
	}

	void actor_manage::statrt_suspend_thread()
	{
		m_impl_actor_manage()->statrt_suspend_thread();
	}

	void actor_manage::finish_suspend_thread()
	{
		m_impl_actor_manage()->finish_suspend_thread();
	}
}//namespace ngl