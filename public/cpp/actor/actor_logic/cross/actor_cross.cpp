#include "actor_cross.h"

namespace ngl
{
	actor_cross::actor_cross() :
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

	void actor_cross::init()
	{
	}

	actor_cross::~actor_cross() {}

	void actor_cross::nregister()
	{
		// 定时器
		actor_cross::register_timer<actor_cross>(&actor_cross::timer_handle);
	}

	i64_actorid actor_cross::actorid()
	{
		return nguid::make(ACTOR_CROSS, nguid::none_area(), nguid::none_actordataid());
	}

	void actor_cross::loaddb_finish(bool adbishave)
	{
		LogLocalError("actor_cross###loaddb_finish#[%]", nguid(id_guid()));
	}
	
	bool actor_cross::timer_handle(message<timerparm>& adata)
	{
		return true;
	}
}//namespace ngl