#include "actor_csvserver.h"

namespace ngl
{
	actor_csvserver::actor_csvserver() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVSERVER,
				},
				.m_weight = 0x7fffffff,
			})
	{}

	ENUM_ACTOR actor_csvserver::actor_type()
	{
		return ACTOR_CSVSERVER;
	}

	i64_actorid actor_csvserver::actorid()
	{
		return nguid::make(ACTOR_CSVSERVER, nguid::none_area(), nguid::none_actordataid());
	}

	void actor_csvserver::init() 
	{
	}

	void actor_csvserver::nregister()
	{
		register_handle_custom<actor_csvserver>::func<
			np_actor_csv_verify_version
		>(true);
	}
}//namespace ngl