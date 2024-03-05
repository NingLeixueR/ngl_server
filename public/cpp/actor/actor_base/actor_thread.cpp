#include "actor_thread.h"
#include "actor_manage.h"


namespace ngl
{
	struct actor_thread::impl_actor_thread
	{
		i32_threadid		m_id;
		ptractor			m_actor;
		bool				m_isactivity;
		ngl::thread			m_thread;
		std::shared_mutex	m_mutex;
		ngl::sem			m_sem;

		impl_actor_thread(i32_threadid aid, actor_thread* athread) :
			m_id(aid),
			m_actor(nullptr),
			m_isactivity(false),
			m_thread(&actor_thread::run, athread)
		{}

		bool isactivity()
		{
			monopoly_shared_lock(m_mutex);
			return m_isactivity;
		}

		void push(ptractor& abase)
		{
			monopoly_shared_lock(m_mutex);
			m_actor = abase;
			m_isactivity = false;
			m_sem.post();
		}

		void run(actor_thread* athread)
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

	actor_thread::actor_thread(i32_threadid aid)
	{
		m_impl_actor_thread.make_unique(aid, this);
	}

	i32_threadid actor_thread::id()
	{
		return m_impl_actor_thread()->m_id;
	}

	bool actor_thread::isactivity()
	{
		return m_impl_actor_thread()->isactivity();
	}

	void actor_thread::push(ptractor abase)
	{
		m_impl_actor_thread()->push(abase);
	}

	void actor_thread::run()
	{
		m_impl_actor_thread()->run(this);
	}
}