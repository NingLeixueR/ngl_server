#include "actor_manage.h"
#include "ndbclient.h"
#include "actor.h"
#include "net.h"

#include <deque>
#include <queue>

namespace ngl
{
#define DEF_ACTOR_USE_DEQUE 
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
		int								m_weight;	// 权重

		explicit impl_actor(const actorparm& aparm)
			: m_stat(actor_stat_init)
			, m_weight(aparm.m_weight)
		{
		}

		// # 释放actor所持有的资源
		inline void release(actor* aactor)
		{
			actor_handle(nullptr, 0x7fffffff);
			aactor->save();
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
			m_list.push_back(std::move(apram));
		}
	private:
		// # 设置kcp
		inline void set_kcp(actor* aactor, handle_pram& aparm)const
		{
			if (aparm.m_pack != nullptr
				&& aparm.m_pack->m_protocol == ENET_KCP
				&& aactor->is_single() == false
				)
			{
				aactor->set_kcpssion(aparm.m_pack->m_id);
			}
		}

		inline bool ahandle(actor* aactor, i32_threadid athreadid, handle_pram& aparm)const
		{
			if (aactor == nullptr)
				return false;
			Try
			{
				set_kcp(aactor, aparm);
				nrfunbase* lprfun = aactor->m_actorfun[aparm.m_protocoltype];
				Assert(lprfun != nullptr)
				if (lprfun->handle_switch(aactor, athreadid, aparm))
					return true;
				lprfun->notfindfun(aactor, athreadid, aparm);
			}Catch
			return false;
		}
	private:
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
		inline void actor_handle(actor* aactor, i32_threadid athreadid, int aweight)
		{
			if (aactor == nullptr)
				return;
			tls<handle_pram> llist;
			swaplist(llist);
			if (aweight < llist.size())
			{
				ngl::log_error()->print(
					"actor handle weight/count[{}/{}]", 
					aweight, llist.size()
				);
			}
				
			while (--aweight >= 0 && llist.empty() != true)
			{
				handle_pram& lparm = llist.front();
				if (ahandle(aactor, athreadid, lparm) == true)
				{
					aactor->handle_after(lparm);
				}
				llist.pop_front();
			}
			if (llist.empty() == false)
			{
				insertlist(llist);
			}
		}

		inline void actor_handle(actor* aactor, i32_threadid athreadid)
		{
			actor_handle(aactor, athreadid, m_weight);
		}
	};

	actor::actor(const actorparm& aparm) :
		actor_base(aparm.m_parm),
		m_actorfun({nullptr})
	{
		m_impl_actor.make_unique(aparm);
		set_broadcast(aparm.m_broadcast);
	}

	actor::~actor()
	{}

	void actor::release()
	{
		m_impl_actor()->release(this);
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
		m_impl_actor()->actor_handle(this, athreadid);
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