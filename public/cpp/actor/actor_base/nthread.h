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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/actor_base/handle_pram.h"
#include "actor/actor_base/actor_base.h"
#include "tools/threadtools.h"

#include <atomic>

namespace ngl
{
	class nthread :public std::enable_shared_from_this<nthread>
	{
		nthread() = delete;
		nthread(const nthread&) = delete;
		nthread& operator=(const nthread&) = delete;

		i32_threadid		m_id = 0;				// Stable worker index assigned by actor_manage.
		ptractor			m_actor = nullptr;		// Actor currently borrowed by this worker.
		bool				m_isactivity = false;	// Legacy activity flag preserved for diagnostics/API compatibility.
		std::jthread		m_thread;				// Dedicated worker thread.
		std::atomic_bool	m_shutdown = false;		// Prevents double-stop races during teardown.
		std::shared_mutex	m_mutex;				// Protects the current actor pointer and activity flag.
		ngl::sem			m_sem;					// Wakes the worker when a new actor is assigned.

		void run(std::stop_token astop);
	public:
		explicit nthread(i32_threadid aid);
		~nthread();

		// Return the worker id used in actor_handle callbacks.
		i32_threadid id();

		// Return the legacy activity flag for this worker.
		bool isactivity();

		// Stop the worker thread and wait for it to exit.
		void shutdown();

		// Assign an actor to this worker and wake the thread.
		void push(ptractor aactor);
	};
}//namespace ngl
