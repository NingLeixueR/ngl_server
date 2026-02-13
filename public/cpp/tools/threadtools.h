/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

	// # 信号量
	class sem
	{
		sem(const sem&) = delete;
		sem& operator=(const sem&) = delete;

		std::counting_semaphore<1> m_sem;
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

	// # 让当前线程睡眠
	class sleep
	{
		sleep() = delete;
		sleep(const sleep&) = delete;
		sleep& operator=(const sleep&) = delete;
	public:
		// # 线程睡眠[avalue]小时
		static void hours(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::hours(avalue));
		}
		
		// # 线程睡眠[avalue]秒
		static void seconds(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::seconds(avalue));
		}

		// # 线程睡眠[avalue]毫秒
		static void milliseconds(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(avalue));
		}
	};
}// namespace ngl

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define lock_read(mutex) std::shared_lock<std::shared_mutex> CONCAT(__read_lock_, __LINE__)(mutex) 
#define lock_write(mutex)  std::lock_guard<std::shared_mutex> CONCAT(__write_lock_, __LINE__)(mutex)
   
#define monopoly_shared_lock(MUTEX)	lock_write(MUTEX)

// 条件变量
#define monopoly_lock(MUTEX)		std::lock_guard<std::mutex> __Lock__(MUTEX)
#define cv_lock(CV, MUTEX, FUN)							\
	std::unique_lock<std::mutex> __Lock__(m_mutex);		\
	if (!FUN())											\
	{													\
		CV.wait(__Lock__, FUN);							\
	}

// # 使用信号量/条件变量
#define OPEN_SEM

#ifdef OPEN_SEM
# define ngl_lockinit							\
		std::shared_mutex			m_mutex;	\
		ngl::sem					m_sem
#else
# define ngl_lockinit							\
		std::mutex					m_mutex;	\
		std::condition_variable		m_cv
#endif//OPEN_SEM

#ifdef OPEN_SEM
# define ngl_lock monopoly_shared_lock(m_mutex)
#else
# define ngl_lock monopoly_lock(m_mutex)
#endif//OPEN_SEM

// 用于检查死锁
#ifdef DECHECK_LOCK_TAR
# define ngl_lock_s std::cout << std::format("lock_open:{},{}", __FILE__,__LINE__) << std::endl;\
ngl_lock;\
std::cout << std::format("lock_close:{},{}", __FILE__,__LINE__) << std::endl
#else
# define ngl_lock_s ngl_lock
#endif//DECHECK_LOCK_TAR

#ifdef OPEN_SEM
# define ngl_post m_sem.post()
#else
# define ngl_post m_cv.notify_one()
#endif//OPEN_SEM
