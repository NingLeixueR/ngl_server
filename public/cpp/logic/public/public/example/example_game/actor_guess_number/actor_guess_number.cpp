#include "actor_guess_number.h"

namespace ngl
{
	actor_guess_number::actor_guess_number(const std::set<i64_actorid>& aroleids, int32_t aindex) :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_EXAMPLE_GUESS_NUMBER,
						.m_area = tab_self_area,
						.m_id	= aindex
					},
				.m_weight = 0x7fffffff,
			}),
		m_rolesds(aroleids)
	{

	}

}//namespace ngl