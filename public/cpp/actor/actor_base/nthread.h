#pragma once

#include "threadtools.h"
#include "handle_pram.h"
#include "actor_base.h"
#include "impl.h"

namespace ngl
{
	class nthread
	{
		struct impl_nthread;
		ngl::impl<impl_nthread> m_impl_actor_thread;
	public:
		nthread(i32_threadid aid);
		i32_threadid id();
		bool isactivity();
		void push(ptractor abase);
		void run();
	};
}//namespace ngl



