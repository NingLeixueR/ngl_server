#include "actor_manage.h"
#include "nthread.h"

namespace ngl
{
	struct nthread::impl_nthread
	{
		i32_threadid		m_id;
		ptractor			m_actor;
		bool				m_isactivity;
		ngl::thread			m_thread;
		std::shared_mutex	m_mutex;
		ngl::sem			m_sem;

		inline impl_nthread(i32_threadid aid, nthread* athread) :
			m_id(aid),
			m_actor(nullptr),
			m_isactivity(false),
			m_thread(&nthread::run, athread)
		{}

		inline bool isactivity()
		{
			monopoly_shared_lock(m_mutex);
			return m_isactivity;
		}

		inline void push(ptractor& abase)
		{
			monopoly_shared_lock(m_mutex);
			m_actor = abase;
			m_isactivity = false;
			m_sem.post();
		}

		inline void run(nthread* athread)
		{
			ptractor lpactor = nullptr;
			while (true)
			{
				m_sem.wait();
				{
					monopoly_shared_lock(m_mutex);
					lpactor = m_actor;
				}
				if (lpactor != nullptr && !lpactor->list_empty())
				{
					lpactor->actor_handle(m_id);
					{
						monopoly_shared_lock(m_mutex);
						m_actor = nullptr;
						m_isactivity = false;
					}
					actor_manage::getInstance().push(lpactor, athread);
				}
			}
		}
	};

	nthread::nthread(i32_threadid aid)
	{
		m_impl_actor_thread.make_unique(aid, this);
	}

	i32_threadid nthread::id()
	{
		return m_impl_actor_thread()->m_id;
	}

	bool nthread::isactivity()
	{
		return m_impl_actor_thread()->isactivity();
	}

	void nthread::push(ptractor abase)
	{
		m_impl_actor_thread()->push(abase);
	}

	void nthread::run()
	{
		m_impl_actor_thread()->run(this);
	}
}//namespace ngl