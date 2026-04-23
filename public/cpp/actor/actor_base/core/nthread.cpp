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
// File overview: Actor worker thread implementation.
//
// The worker loop: wait on sem → process actor messages → return worker
// and actor to actor_manage via push(). The thread is detached at
// construction and runs for the process lifetime.

#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/nthread.h"

namespace ngl
{
	nthread::nthread(i32_threadid aid) :
		m_id(aid),
		m_actor(nullptr),
		m_isactivity(false),
		m_thread([this]()
			{
				run();
			})
	{
		m_thread.detach();
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
			return;
		}
		{
			lock_write(m_mutex);
			m_actor = aactor;
			m_isactivity = true;
		}
		m_sem.post();
	}

	void nthread::run()
	{
		ptractor lpactor = nullptr;
		for (;;)
		{
			m_sem.wait();
			{
				lock_write(m_mutex);
				lpactor = m_actor;
			}
			if (lpactor == nullptr)
			{
				continue;
			}

			bool lready = false;
			if (!lpactor->list_empty())
			{
				// An actor handles a batch of queued messages on exactly one worker at a time.
				lready = lpactor->actor_handle(m_id);
			}

			{
				lock_write(m_mutex);
				m_actor = nullptr;
				m_isactivity = false;
			}

			// Return both the actor and this worker to actor_manage so it can decide
			// whether the actor should be rescheduled immediately.
			actor_manage::instance().push(lpactor, this, lready);
			lpactor = nullptr;
		}
	}
}//namespace ngl
