#include "actor.h"
#include "net.h"
#include "actor_manage.h"

namespace ngl
{
	struct actor::impl_actor
	{
		std::list<handle_pram> m_list;
		actor_stat m_stat;
		std::shared_mutex m_mutex;
		int m_weight;

		impl_actor(const actorparm& aparm)
			: m_weight(aparm.m_weight)
			, m_stat(actor_stat_init)
		{
		}

		inline void release(actor* aactor)
		{
			actor_handle(0, 0x7fffffff);
			aactor->save();
			m_list.clear();
		}

		inline bool list_empty()
		{
			monopoly_shared_lock(m_mutex);
			return m_list.empty();
		}

		inline actor_stat get_activity_stat()
		{
			monopoly_shared_lock(m_mutex);
			return m_stat;
		}

		inline void set_activity_stat(actor_stat astat)
		{
			monopoly_shared_lock(m_mutex);
			m_stat = astat;
		}

		inline void push(handle_pram& apram)
		{
			monopoly_shared_lock(m_mutex);
			m_list.push_back(std::move(apram));
		}

	private:
		inline bool ahandle(actor* aactor, i32_threadid athreadid, handle_pram& aparm)
		{
			if (aactor == nullptr)
				return false;
			Try
			{
				if (aparm.m_pack != nullptr 
					&& aparm.m_pack->m_protocol == ENET_KCP
					&& aactor->is_single() == false
				)
				{
					aactor->set_kcpssion(aparm.m_pack->m_id);
				}
				arfunbase * lprfun = aactor->m_actorfun[aparm.m_protocoltype];
				Assert(lprfun != nullptr);
				if (lprfun->handle_switch(aactor, athreadid, aparm))
					return true;
				lprfun->notfindfun(aactor, athreadid, aparm);
			}Catch;
			return false;
		}
	public:
		inline void actor_handle(actor* aactor, i32_threadid athreadid, int aweight)
		{
			std::list<handle_pram> llist;
			{
				monopoly_shared_lock(m_mutex);
				llist.swap(m_list);
			}
			while (--aweight >= 0 && llist.empty() != true)
			{
				if (ahandle(aactor, athreadid, *llist.begin()) == true)
				{
					aactor->handle_after();
				}
				llist.pop_front();
			}

			{
				monopoly_shared_lock(m_mutex);
				m_list.insert(m_list.begin(), llist.begin(), llist.end());
			}
		}

		inline void actor_handle(actor* aactor, i32_threadid athreadid)
		{
			actor_handle(aactor, athreadid, m_weight);
		}
	};

	actor::actor(const actorparm& aparm) :
		actor_base(aparm.m_parm)
	{
		m_impl_actor.make_unique(aparm);
		set_broadcast(aparm.m_broadcast);
	}

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

	void actor::clear_task() {}

	void actor::actor_handle(i32_threadid athreadid)
	{
		m_impl_actor()->actor_handle(this, athreadid);
	}

	actor::~actor() 
	{
	}
}