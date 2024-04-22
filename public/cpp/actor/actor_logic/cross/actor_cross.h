#pragma once

#include "actor.h"

namespace ngl
{
	class actor_cross : public actor
	{
		actor_cross() :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = ACTOR_CROSS,
						.m_area = nguid::none_area(),// 跨服此值无意义
					},
					.m_weight = 0x7fffffff,
				})
		{
		}
	public:
		friend class actor_instance<actor_cross>;
		static actor_cross& getInstance()
		{
			return actor_instance<actor_cross>::instance();
		}
		
		virtual void init()
		{
		}

		virtual ~actor_cross() {}

		static void nregister();

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_CROSS, nguid::none_area(), nguid::none_actordataid());
		}

		virtual void loaddb_finish(bool adbishave);

		// 定时器
		bool timer_handle(message<timerparm>& adata);
	};
}//namespace ngl

