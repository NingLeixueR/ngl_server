#pragma once

#include "actor.h"

namespace ngl
{
	class actor_cross : public actor
	{
		actor_cross(const actor_cross&) = delete;
		actor_cross& operator=(const actor_cross&) = delete;

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

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_CROSS;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_CROSS, nguid::none_area(), nguid::none_actordataid());
		}

		virtual void loaddb_finish(bool adbishave);

		bool timer_handle(const message<np_timerparm>& adata);
	};
}//namespace ngl

