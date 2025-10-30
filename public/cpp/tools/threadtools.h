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
	using thread			= std::thread;

	// # �ź���
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

		virtual ~sem()
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

	// # �õ�ǰ�߳�˯��
	class sleep
	{
		sleep() = delete;
		sleep(const sleep&) = delete;
		sleep& operator=(const sleep&) = delete;
	public:
		// # �߳�˯��[avalue]Сʱ
		static void hours(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::hours(avalue));
		}
		
		// # �߳�˯��[avalue]��
		static void seconds(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::seconds(avalue));
		}

		// # �߳�˯��[avalue]����
		static void milliseconds(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(avalue));
		}
	};
}// namespace ngl

#define lock_read(MUTEX)			std::shared_lock<std::shared_mutex> __Lock__(MUTEX)
#define lock_write(MUTEX)			std::lock_guard<std::shared_mutex> __Lock__(MUTEX)
#define monopoly_shared_lock(MUTEX)	lock_write(MUTEX)

// ��������
#define monopoly_lock(MUTEX)		std::lock_guard<std::mutex> __Lock__(MUTEX)
#define cv_lock(CV, MUTEX, FUN)							\
	std::unique_lock<std::mutex> __Lock__(m_mutex);		\
	if (!FUN())											\
	{													\
		CV.wait(__Lock__, FUN);							\
	}

// # ʹ���ź���/��������
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

// ���ڼ������
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
