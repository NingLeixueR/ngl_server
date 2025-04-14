#include "actor_brief.h"
#include "net.pb.h"
#include "nsp_server.h"

namespace ngl
{
	actor_brief::actor_brief() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_BRIEF,
						.m_area = tab_self_area,
						.m_manage_dbclient = true
					},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		tdb_brief::nsp_ser::init(&m_briefdb);
	}

	void actor_brief::init()
	{
		m_briefdb.set(this);
	}

	void actor_brief::loaddb_finish(bool adbishave)
	{
		log_error()->print("actor_brief::loaddb_finish({})", adbishave ? "true" : "false");
	}

	ENUM_ACTOR actor_brief::actor_type()
	{
		return ACTOR_BRIEF;
	}

	i64_actorid actor_brief::actorid()
	{
		return nguid::make(ACTOR_BRIEF, tab_self_area, nguid::none_actordataid());
	}

	void actor_brief::nregister()
	{		
	}
}//namespace ngl