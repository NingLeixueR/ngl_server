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

namespace ngl
{
	class nthread :public std::enable_shared_from_this<nthread>
	{
		nthread() = delete;
		nthread(const nthread&) = delete;
		nthread& operator=(const nthread&) = delete;

		i32_threadid		m_id = 0;				// Threadid
		ptractor			m_actor = nullptr;		// This thread actor
		bool				m_isactivity = false;	// Threadwhether
		ngl::thread			m_thread;				// Thread
		std::shared_mutex	m_mutex;				// Translated comment.
		ngl::sem			m_sem;					// Semaphore

		void run();
	public:
		explicit nthread(i32_threadid aid);

		// # Returnthreadid
		i32_threadid id();

		// # Whether
		bool isactivity();

		// # Tothreadinaddactor
		void push(ptractor aactor);
	};
}//namespace ngl