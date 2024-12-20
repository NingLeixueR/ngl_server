#include "actor_manage.h"
#include "ndbclient.h"
#include "actor.h"
#include "net.h"

#include <deque>
#include <queue>

namespace ngl
{
	struct actor::impl_actor
	{
		impl_actor() = delete;
		impl_actor(const impl_actor&) = delete;
		impl_actor& operator=(const impl_actor&) = delete;

#ifdef DEF_ACTOR_USE_DEQUE
		template <typename T>
		using tls = std::deque<T>;
#else
		template <typename T>
		using tls = std::list<T>;
#endif//DEF_ACTOR_USE_LIST

		tls<handle_pram>				m_list;		// 待处理消息列表
		actor_stat						m_stat;		// actor状态
		std::shared_mutex				m_mutex;	// 锁:[m_list:待处理消息列表]
		int32_t							m_weight;	// 权重
		int32_t							m_timeout;	// 超时:(当actor处理消息超过此时间)
		bool							m_release;  // 释放将忽略权重和超时
		actor*							m_actor;

		explicit impl_actor(const actorparm& aparm, actor* aactor)
			: m_stat(actor_stat_init)
			, m_weight(aparm.m_weight)
			, m_timeout(aparm.m_timeout)
			, m_release(false)
			, m_actor(aactor)
		{
		}

		// # 释放actor所持有的资源
		inline void release()
		{
			m_release = true;
			actor_handle(0x7fffffff);
			m_actor->save();
			m_list.clear();
		}

		// # actor 消息列表是否为空
		inline bool list_empty()
		{
			monopoly_shared_lock(m_mutex);
			return m_list.empty();
		}

		// # 获取actor状态
		inline actor_stat get_activity_stat()
		{
			monopoly_shared_lock(m_mutex);
			return m_stat;
		}

		// # 设置actor状态
		inline void set_activity_stat(actor_stat astat)
		{
			monopoly_shared_lock(m_mutex);
			m_stat = astat;
		}

		// # 向actor消息列表中添加消息
		inline void push(handle_pram& apram)
		{
			monopoly_shared_lock(m_mutex);
			m_list.push_back(apram);
		}
	private:
		// # 设置kcp
		inline void set_kcp(handle_pram& aparm)
		{
			if (aparm.m_pack == nullptr)
				return;
			if (aparm.m_pack->m_protocol != ENET_KCP)
				return;
			if (m_actor->is_single())
				return;
			m_actor->set_kcpssion(aparm.m_pack->m_id);
		}

		inline bool ahandle(i32_threadid athreadid, handle_pram& aparm)
		{
			if (m_actor == nullptr)
				return false;
			Try
			{
				set_kcp(aparm);
				nrfunbase* lprfun = m_actor->m_actorfun[aparm.m_protocoltype];
				Assert(lprfun != nullptr)
				if (lprfun->handle_switch(m_actor, athreadid, aparm))
					return true;
				lprfun->notfindfun(m_actor, athreadid, aparm);
			}Catch
			return false;
		}

		inline void swaplist(tls<handle_pram>& als)
		{
			monopoly_shared_lock(m_mutex);
			m_list.swap(als);
		}

		inline void insertlist(tls<handle_pram>& als)
		{
			monopoly_shared_lock(m_mutex);
			m_list.insert(m_list.begin(), als.begin(), als.end());
		}
	public:
		inline void actor_handle(i32_threadid athreadid, int aweight)
		{
			if (m_actor == nullptr)
				return;
			tls<handle_pram> llist;
			swaplist(llist);
			if (aweight < llist.size())
			{
				m_actor->log_error()->print("actor handle [weight:{}/count:{}]", aweight, llist.size());
			}
			time_t lbeg = localtime::gettimems();
			int32_t lcount = 0;
			while (--aweight >= 0 && llist.empty() != true)
			{
				if (m_release == false && localtime::gettimems()- lbeg> m_timeout)
				{
					break;
				}
				handle_pram& lparm = llist.front();
				if (ahandle(athreadid, lparm) == true)
				{
					m_actor->handle_after(lparm);
				}
				llist.pop_front();
				++lcount;
			}
			if (llist.empty() == false)
			{
				insertlist(llist);
			}
		}

		inline void actor_handle(i32_threadid athreadid)
		{
			actor_handle(athreadid, m_weight);
		}
	};

	actor::actor(const actorparm& aparm) :
		actor_base(aparm.m_parm),
		m_actorfun({nullptr})
	{
		m_impl_actor.make_unique(aparm, this);
		set_broadcast(aparm.m_broadcast);
	}

	actor::~actor()
	{}

	void actor::release()
	{
		m_impl_actor()->release();
	}

	bool actor::list_empty()
	{
		return m_impl_actor()->list_empty();
	}

	actor_stat actor::get_activity_stat()
	{
		return m_impl_actor()->get_activity_stat();
	}

	void actor::set_activity_stat(actor_stat astat)
	{
		m_impl_actor()->set_activity_stat(astat);
	}

	void actor::push(handle_pram& apram)
	{
		m_impl_actor()->push(apram);
	}

	void actor::actor_handle(i32_threadid athreadid)
	{
		m_impl_actor()->actor_handle(athreadid);
	}

	bool actor::handle(const message<np_actor_broadcast>& adata)
	{
		// # 保存数据
		if (get_actor_manage_dbclient() != nullptr)
		{
			get_actor_manage_dbclient()->save();
			get_actor_manage_dbclient()->del();
		}
		// # actor派生类自定义处理逻辑
		broadcast();
		return true;
	}
}//namespace ngl