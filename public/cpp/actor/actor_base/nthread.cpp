#include "actor_manage.h"
#include "nthread.h"

namespace ngl
{
	nthread::nthread(i32_threadid aid) :
		m_id(aid),
		m_actor(nullptr),
		m_isactivity(false),
		m_thread(&nthread::run, this)
	{
	}

	i32_threadid nthread::id()
	{
		return m_id;
	}

	bool nthread::isactivity()
	{
		monopoly_shared_lock(m_mutex);
		return m_isactivity;
	}

	void nthread::push(ptractor aactor)
	{
		monopoly_shared_lock(m_mutex);
		m_actor = aactor;
		m_isactivity = false;
		m_sem.post();
	}

	void nthread::run()
	{
		ptractor lpactor = nullptr;
		while (true)
		{
			m_sem.wait();
			{
				monopoly_shared_lock(m_mutex);
				lpactor = m_actor;
			}
			if (lpactor != nullptr)
			{
				if (!lpactor->list_empty())
				{
					lpactor->actor_handle(m_id);
					{
						monopoly_shared_lock(m_mutex);
						m_actor = nullptr;
						m_isactivity = false;
					}
				}
				actor_manage::instance().push(lpactor, this);
			}
		}
	}
}//namespace ngl