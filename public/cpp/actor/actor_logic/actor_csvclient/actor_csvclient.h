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

		virtual ~actor_csvclient() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		bool handle(const message<np_actor_reloadcsv>& adata);

		// # ¶¨Ê±Æ÷
		bool timer_handle(const message<np_timerparm>& adata);
	};
}//namespace ngl