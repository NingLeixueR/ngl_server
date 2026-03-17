/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for actor base.

#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/nthread.h"

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
		lock_read(m_mutex);
		return m_isactivity;
	}

	void nthread::push(ptractor aactor)
	{
		if (aactor == nullptr)
		{
			tools::no_core_dump();
		}
		lock_write(m_mutex);
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
				lock_write(m_mutex);
				lpactor = m_actor;
			}
			if (lpactor != nullptr)
			{
				if (!lpactor->list_empty())
				{
					// An actor handles a batch of queued messages on exactly one worker at a time.
					lpactor->actor_handle(m_id);
					{
						lock_write(m_mutex);
						m_actor = nullptr;
						m_isactivity = false;
					}
				}
				// Return both the actor and this worker to actor_manage so it can decide
				// whether the actor should be rescheduled immediately.
				actor_manage::instance().push(lpactor, shared_from_this());
			}
		}
	}
}//namespace ngl
