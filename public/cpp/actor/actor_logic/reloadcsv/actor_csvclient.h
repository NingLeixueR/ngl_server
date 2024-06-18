#pragma once

#include "actor.h"

namespace ngl
{
	class actor_csvclient 
		: public actor
	{
		actor_csvclient(const actor_csvclient&) = delete;
		actor_csvclient& operator=(const actor_csvclient&) = delete;

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

		// # ¶¨Ê±Æ÷
		bool timer_handle(message<timerparm>& adata);
	};
}//namespace ngl