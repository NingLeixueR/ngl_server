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

		i32_threadid		m_id;
		ptractor			m_actor;
		bool				m_isactivity;
		ngl::thread			m_thread;
		std::shared_mutex	m_mutex;
		ngl::sem			m_sem;

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



