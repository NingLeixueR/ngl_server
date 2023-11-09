#pragma once

#include "actor_base.h"
#include "handle_pram.h"
#include "threadtools.h"
#include "impl.h"

namespace ngl
{
	class actor_thread
	{
		struct impl_actor_thread;
		ngl::impl<impl_actor_thread> m_impl_actor_thread;
	public:
		actor_thread(i32_threadid aid);
		i32_threadid id();
		bool isactivity();
		void push(ptractor abase);
		void run();
	};
}



