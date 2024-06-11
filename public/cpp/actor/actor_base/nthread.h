#pragma once

#include "threadtools.h"
#include "handle_pram.h"
#include "actor_base.h"
#include "impl.h"

namespace ngl
{
	class nthread
	{
		nthread() = delete;
		nthread(const nthread&) = delete;
		nthread& operator=(const nthread&) = delete;

		struct impl_nthread;
		ngl::impl<impl_nthread> m_impl_actor_thread;
	public:
		nthread(i32_threadid aid);
		// # 返回线程id
		i32_threadid id();
		// # 是否活跃
		bool isactivity();
		// # 想线程中添加actor
		void push(ptractor aactor);
	private:
		void run();
	};
}//namespace ngl



