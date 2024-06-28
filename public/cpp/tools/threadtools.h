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
	//using mutex			= std::shared_mutex;
	//using shared_lock		= std::shared_lock<mutex>; // 读锁
	//using unique_lock		= std::unique_lock<mutex>; // 写锁
	using thread			= std::thread;

	class sem
	{
		sem(const sem&) = delete;
		sem& operator=(const sem&) = delete;

		std::counting_semaphore<1> m_sem;
	public:
		sem() :
			m_sem(0)
		{
		}

		~sem()
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

	class sleep
	{
		sleep() = delete;
		sleep(const sleep&) = delete;
		sleep& operator=(const sleep&) = delete;

	public:
		// # 线程睡眠[avalue]小时
		static void seconds(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::seconds(avalue));
		}

		// # 线程睡眠[avalue]秒
		static void hours(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::hours(avalue));
		}

		// # 线程睡眠[avalue]毫秒
		static void milliseconds(int32_t avalue)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(avalue));
		}
	};
}// namespace ngl

#define lock_read(MUTEX)			std::shared_lock<std::shared_mutex> __Lock__(MUTEX)
#define lock_write(MUTEX)			std::lock_guard<std::shared_mutex> __Lock__(MUTEX)
#define monopoly_shared_lock(MUTEX)	lock_write(MUTEX)

// 条件变量
#define monopoly_lock(MUTEX)		std::lock_guard<std::mutex> __Lock__(MUTEX)
#define cv_lock(CV, MUTEX, FUN)							\
	std::unique_lock<std::mutex> __Lock__(m_mutex);		\
	if (!FUN())											\
	{													\
		CV.wait(__Lock__, FUN);							\
	}

#define OPEN_SEM

#ifdef OPEN_SEM
# define ngl_lockinit							\
		std::shared_mutex			m_mutex;	\
		ngl::sem					m_sem
#else
# define ngl_lockinit							\
		std::mutex					m_mutex;	\
		std::condition_variable		m_cv;
#endif//OPEN_SEM

#ifdef OPEN_SEM
# define ngl_lock monopoly_shared_lock(m_mutex)
#else
# define ngl_lock monopoly_lock(m_mutex)
#endif//OPEN_SEM

#ifdef OPEN_SEM
# define ngl_post m_sem.post()
#else
# define ngl_post m_cv.notify_one()
#endif//OPEN_SEM

namespace ngl
{
	template <typename NODE>
	class work_list
	{
		work_list() = delete;
		work_list(const work_list&) = delete;
		work_list& operator=(const work_list&) = delete;

		std::shared_mutex			m_mutex;
		ngl::sem					m_sem;
		std::list<NODE>				m_list;
		std::function<void(NODE&)>	m_fun;
		std::thread					m_thread;
	public:
		work_list(const std::function<void(NODE&)>& afun) :
			m_fun(afun),
			m_thread([this] { run(); })
		{
		}

		void run()
		{
			std::list<NODE> llist;
			while (true)
			{
				m_sem.wait();
				{
					monopoly_shared_lock(m_mutex);
					if (m_list.empty())
						continue;
					llist.swap(m_list);
				}

				for (auto& item : llist)
				{
					m_fun(item);
				}
				llist.clear();
			}
		}

		inline void push_back(NODE&& anode)
		{
			{
				monopoly_shared_lock(m_mutex);
				m_list.push_back(anode);
			}
			m_sem.post();
		}

		inline void push_back(NODE& anode)
		{
			{
				monopoly_shared_lock(m_mutex);
				m_list.push_back(anode);
			}
			m_sem.post();
		}
	};

	template <typename NODE>
	class workcv_list
	{
		workcv_list() = delete;
		workcv_list(const workcv_list&) = delete;
		workcv_list& operator=(const workcv_list&) = delete;

		std::mutex					m_mutex;
		std::condition_variable		m_cv;
		std::list<NODE>				m_list;
		std::function<void(NODE&)>	m_fun;
		std::thread					m_thread;
	public:
		workcv_list(const std::function<void(NODE&)>& afun) :
			m_fun(afun),
			m_thread([this] { run(); })
		{
		}

		void run()
		{
			std::list<NODE> llist;
			auto lfun = [this]() {return !m_list.empty(); };
			while (true)
			{
				{
					cv_lock(m_cv, m_mutex, lfun)
					llist.swap(m_list);
				}

				for (auto& item : llist)
				{
					m_fun(item);
				}
				llist.clear();
			}
		}

		inline void push_back(NODE&& anode)
		{
			{
				monopoly_lock(m_mutex);
				m_list.push_back(anode);
				m_cv.notify_one();
			}
		}

		inline void push_back(NODE& anode)
		{
			{
				monopoly_lock(m_mutex);
				m_list.push_back(anode);
				m_cv.notify_one();
			}
		}
	};
}// namespace ngl
