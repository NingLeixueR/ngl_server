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
// File overview: Implements actor worker-thread scheduling.

#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/nthread.h"

namespace ngl
{
	nthread::nthread(i32_threadid aid) :
		m_id(aid),
		m_actor(nullptr),
		m_isactivity(false),
		m_thread([this](std::stop_token astop)
			{
				run(astop);
			})
	{
	}

	nthread::~nthread()
	{
		shutdown();
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

	void nthread::shutdown()
	{
		bool lexpected = false;
		if (!m_shutdown.compare_exchange_strong(lexpected, true, std::memory_order_relaxed))
		{
			return;
		}

		m_thread.request_stop();
		m_sem.post();
		if (m_thread.joinable())
		{
			m_thread.join();
		}

		lock_write(m_mutex);
		m_actor = nullptr;
		m_isactivity = false;
	}

	void nthread::push(ptractor aactor)
	{
		if (aactor == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		lock_write(m_mutex);
		if (m_shutdown.load(std::memory_order_relaxed))
		{
			return;
		}
		m_actor = aactor;
		m_isactivity = true;
		m_sem.post();
	}

	void nthread::run(std::stop_token astop)
	{
		ptractor lpactor = nullptr;
		while (!astop.stop_requested())
		{
			m_sem.wait();
			if (astop.stop_requested())
			{
				break;
			}
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

			if (astop.stop_requested())
			{
				break;
			}

			// Return both the actor and this worker to actor_manage so it can decide
			// whether the actor should be rescheduled immediately.
			actor_manage::instance().push(lpactor, shared_from_this(), lready);
			lpactor = nullptr;
		}
	}
}//namespace ngl
