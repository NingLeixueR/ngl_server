#include "actor_csvclient.h"
#include "actor_csvserver.h"
#include "actor_manage.h"

namespace ngl
{
	actor_csvclient::actor_csvclient() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVCLIENT,
					.m_area = tab_self_area,
					.m_id	= nconfig::m_nodeid
				},
				.m_weight = 0x7fffffff,
			})
	{}

	void actor_csvclient::init()
	{
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 5*60) == false)
		{
			log_error()->print("actor_csvclient::init() make_timerparm::make_interval(tparm, 60) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	ENUM_ACTOR actor_csvclient::actor_type()
	{
		return ACTOR_CSVCLIENT;
	}

	i64_actorid actor_csvclient::actorid()
	{
		return nguid::make(ACTOR_CSVCLIENT, tab_self_area, nconfig::m_nodeid);
	}

	void actor_csvclient::nregister()
	{
		actor_csvclient::register_timer<actor_csvclient>(&actor_csvclient::timer_handle);
		
		register_handle_custom<actor_csvclient>::func<
			np_actor_reloadcsv
		>(true);
	}
}//namespace ngl