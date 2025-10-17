#pragma once

#include "threadtools.h"
#include "handle_pram.h"
#include "actor_base.h"

namespace ngl
{
	class nthread
	{
		nthread() = delete;
		nthread(const nthread&) = delete;
		nthread& operator=(const nthread&) = delete;

		i32_threadid		m_id;			// 线程id
		ptractor			m_actor;		// 此时占用线程的actor
		bool				m_isactivity;	// 线程是否活跃
		ngl::thread			m_thread;		// 线程
		std::shared_mutex	m_mutex;		// 互斥量
		ngl::sem			m_sem;			// 信号量

	public:
		nthread(i32_threadid aid);

		// # 返回线程id
		i32_threadid id();

		// # 是否活跃
		bool isactivity();

		// # 向线程中添加actor
		void push(ptractor aactor);

	private:
		void run();
	};
}//namespace ngl



