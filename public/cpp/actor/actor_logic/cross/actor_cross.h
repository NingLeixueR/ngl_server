#pragma once

#include "actor.h"

namespace ngl
{
	class actor_cross : public actor
	{
		actor_cross();
	public:
		friend class actor_instance<actor_cross>;
		static actor_cross& getInstance()
		{
			return actor_instance<actor_cross>::instance();
		}
		
		virtual void init();

		virtual ~actor_cross();

		static void nregister();

		static i64_actorid actorid();

		virtual void loaddb_finish(bool adbishave);

		bool timer_handle(message<timerparm>& adata);
	};
}//namespace ngl

