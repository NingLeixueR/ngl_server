#pragma once

#include "actor.h"

namespace ngl
{
	class actor_csvclient 
		: public actor
	{
		actor_csvclient();
	public:
		friend class actor_instance<actor_csvclient>;
		static actor_csvclient& getInstance()
		{
			return actor_instance<actor_csvclient>::instance();
		}

		virtual void init();

		static void nregister();

		virtual ~actor_csvclient();

		bool handle(message<np_actor_reloadcsv>& adata);

		// 定时器
		// 间接定时器 
		bool timer_handle(message<timerparm>& adata);
	};
}//namespace ngl