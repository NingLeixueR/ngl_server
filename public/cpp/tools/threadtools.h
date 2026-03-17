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
// File overview: Declares interfaces for tools.

#pragma once

#include <condition_variable>
#include <shared_mutex>
#include <functional>
#include <semaphore>
#include <iostream>
#include <thread>
#include <mutex>
#include <list>

namespace ngl
{
	using thread = std::thread;

	// Small counting semaphore wrapper used by worker queues and callback threads.
	class sem
	{
		sem(const sem&) = delete;
		sem& operator=(const sem&) = delete;

		std::counting_semaphore<0x7fffffff> m_sem;
	public:
		inline sem() :
			m_sem(0)
		{
		}

		inline void wait()
		{
			m_sem.acquire();
		}

		inline void post()
		{
			m_sem.release();
		}
	};

	// Sleep helpers used to make call sites read like the intended unit.
	class sleep
	{
		sleep() = delete;
		sleep(const sleep&) = delete;
		sleep& operator=(const sleep&) = delete;
	public:
		// Sleep the current thread for the requested number of hours.
		static void hours(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::hours(avalue));
		}
		
		// Sleep the current thread for the requested number of seconds.
		static void seconds(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::seconds(avalue));
		}

		// Sleep the current thread for the requested number of milliseconds.
		static void milliseconds(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(avalue));
		}
	};
}// namespace ngl

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define lock_read(MUTEX)		std::shared_lock<std::shared_mutex> CONCAT(__read_lock_, __LINE__)(MUTEX) 
#define lock_write(MUTEX)		std::lock_guard<std::shared_mutex> CONCAT(__write_lock_, __LINE__)(MUTEX)

// Optional debug lock macro that prints acquisition/release when
// `DECHECK_LOCK_TAR` is enabled.
#ifdef DECHECK_LOCK_TAR
# define nlock(MUTEX) \
	std::cout << std::format("lock_open:{},{}", __FILE__,__LINE__) << std::endl;	\
	lock_write(MUTEX);																\
	std::cout << std::format("lock_close:{},{}", __FILE__,__LINE__) << std::endl
#else
#	define nlock(MUTEX) lock_write(MUTEX)
#endif//DECHECK_LOCK_TAR
