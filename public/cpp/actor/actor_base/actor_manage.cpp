#include "actor_client.h"
#include "actor_server.h"
#include "actor_manage.h"
#include "actor_db.h"
#include "tools.h"

namespace ngl
{
	struct actor_manage::impl_actor_manage
	{
		impl_actor_manage(const impl_actor_manage&) = delete;
		impl_actor_manage& operator=(const impl_actor_manage&) = delete;

		//// ---- 线程相关
		std::list<nthread*>	m_workthread;		// 工作线程
		bool				m_suspend = false;	// 是否挂起
		std::list<nthread*>	m_suspendthread;	// 挂起的工作线程
		std::jthread		m_thread;			// 管理线程
		i32_threadsize		m_threadnum = -1;	// 工作线程数量

		ngl_lockinit;

		// # 索引actor
		std::map<nguid, ptractor>							m_actorbyid;
		// # 支持广播的actor
		std::map<nguid, ptractor>							m_actorbroadcast;
		// # 按类型索引actor
		std::map<ENUM_ACTOR, std::map<nguid, ptractor>>		m_actorbytype;
		// # 有任务的actor列表
		std::list<ptractor>									m_actorlist;
		// # 包含哪些actortype
		std::set<i16_actortype>								m_actortype;
		// # 删除actor后需要执行的操作(延迟操作:删除的瞬间actor正是运行状态,等待其回归后进行删除)
		std::map<nguid, std::function<void()>>				m_delactorfun;

		impl_actor_manage() :
			m_thread(&impl_actor_manage::run, this)
		{}

		inline void init(i32_threadsize apthreadnum)
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

		inline void get_type(std::vector<i16_actortype>& aactortype) const
		{
			for (i16_actortype item : m_actortype)
			{
				aactortype.push_back(item);
			}
		}

		inline int32_t actor_count()
		{
			ngl_lock_s;
			return (int32_t)m_actorbyid.size();
		}

		inline void get_actor_stat(msg_actor_stat& adata)
		{
			ngl_lock_s;
			std::ranges::for_each(m_actorbytype, [&adata](const auto& apair)
				{
					msg_actor ltemp;
					ltemp.m_actor_name = em<ENUM_ACTOR>::get_name(apair.first);
					std::ranges::for_each(apair.second, [&ltemp](const auto& aguidprt)
						{
							ltemp.m_actor[nguid::area(aguidprt.first)].push_back(nguid::actordataid(aguidprt.first));
						});
					adata.m_vec.push_back(ltemp);
				});
		}

		// 当前进程是ACTORSERVER返回"actor_server::actorid()"否则返回"actor_client::actorid()"
		inline nguid nodetypebyguid()
		{
			return xmlnode::m_nodetype == ACTORSERVER ? actor_server::actorid() : actor_client::actorid();
		}

		inline bool add_actor(const ptractor& apactor, const std::function<void()>& afun)
		{
			const nguid& guid = apactor->guid();
			{
				ngl_lock_s;
				if (m_actorbyid.contains(guid))
				{
					std::cout << std::format("impl_actor_manage add_actor m_actorbyid.contains(guid:{}) fail", guid) << std::endl;
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
				push_task_id(nodetypebyguid(), pro, false);
			}
			apactor->set_activity_stat(actor_stat_free);
			return true;
		}

		inline void erase_actor(const nguid& aguid, const std::function<void()>& afun)
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
			push_task_id(nodetypebyguid(), pro, false);

			bool isrunfun = false;
			ptractor lpactor = nullptr;
			{
				ngl_lock_s;
				ptractor* lpactorptr = tools::findmap(m_actorbyid, aguid);
				if (lpactorptr == nullptr)
				{
					std::cout << std::format("impl_actor_manage erase_actor_byid m_actorbyid.find(guid:{}) fail", aguid) << std::endl;
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

		inline ptractor& nosafe_get_actor(const nguid& aguid)
		{
			static ptractor lnullptr(nullptr);
			auto itor = m_actorbyid.find(aguid);
			if (itor == m_actorbyid.end())
			{
				return lnullptr;
			}
			return itor->second;
		}

		inline ptractor& get_actor(const nguid& aguid)
		{
			ngl_lock_s;
			return nosafe_get_actor(aguid);
		}

		inline bool is_have_actor(const nguid& aguid)
		{
			ngl_lock_s;
			return m_actorbyid.contains(aguid);
		}
		
		// # 将thread与actor一起交给impl_actor_manage
		inline void push(const ptractor& apactor, nthread* atorthread)
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
						ngl_post;
					}
					lrelease = true;
				}
				else
				{
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
				}
			}
			
			if (lrelease)
			{
				apactor->release();
				if (lfun != nullptr)
				{
					lfun();
				}
			}

		}

		// # nosafe_开头的函数代表"内部操作未加锁"，不允许类外调用
		inline void nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram)
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

		inline ptractor& nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram, bool abool)
		{
			static ptractor lnull(nullptr);
			ptractor* lpactorptr = tools::findmap(m_actorbyid, aguid); 
			if (lpactorptr == nullptr)
			{
				if (!abool)
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

		template <typename T>
		inline void push_task_id(const nguid& aguid, std::shared_ptr<T>& apram, bool abool)
		{
			handle_pram lparm = handle_pram::create(aguid, nguid::make(), apram);
			push_task_id(aguid, lparm, abool);
		}

		inline void push_task_id(const nguid& aguid, handle_pram& apram, bool abool)
		{
			ngl_lock_s;
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
			ngl_lock_s;
			// 1.先发给本机上的atype
			for (const auto& [key, value] : m_actorbytype[atype])
			{
				if (value->get_activity_stat() != actor_stat_close)
				{
					nosafe_push_task_id(value, apram);
				}
			}
			ngl_post;
			// 2.然后发给actor_client，发给其他服务器
			if (aotherserver == true)
			{
				nguid lguid = nodetypebyguid();
				ptractor* lpptractor = tools::findmap(m_actorbyid, lguid);
				if (lpptractor == nullptr)
				{
					return;
				}
				nosafe_push_task_id(*lpptractor, apram);
			}
		}

		inline void broadcast_task(handle_pram& apram)
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

		inline void statrt_suspend_thread()
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

		inline void finish_suspend_thread()
		{
			ngl_lock_s;
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

	bool actor_manage::add_actor(const ptractor& apactor, const std::function<void()>& afun)
	{
		return m_impl_actor_manage()->add_actor(apactor, afun);
	}

	bool actor_manage::add_actor(actor_base* apactor, const std::function<void()>& afun)
	{
		ptractor ltemp(apactor);
		return add_actor(ltemp, afun);
	}

	void actor_manage::erase_actor(const nguid& aguid, const std::function<void()>& afun /*= nullptr*/)
	{
		m_impl_actor_manage()->erase_actor(aguid, afun);
	}

	bool actor_manage::is_have_actor(const nguid& aguid)
	{
		return m_impl_actor_manage()->is_have_actor(aguid);
	}

	void actor_manage::push(const ptractor& apactor, nthread* atorthread/* = nullptr*/)
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

	int32_t actor_manage::actor_count()
	{
		return m_impl_actor_manage()->actor_count();
	}

	void actor_manage::get_actor_stat(msg_actor_stat& adata)
	{
		m_impl_actor_manage()->get_actor_stat(adata);
	}

	actor_suspendthread::actor_suspendthread()
	{
		log_info()->print("start actor_suspendthread");
		actor_manage::getInstance().statrt_suspend_thread();
	}

	actor_suspendthread::~actor_suspendthread()
	{
		Try
		{
			actor_manage::getInstance().finish_suspend_thread();
			log_info()->print("finish actor_suspendthread");
		}Catch
	}
}//namespace ngl