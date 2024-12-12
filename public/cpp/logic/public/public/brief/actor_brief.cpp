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

	void actor_brief::nregister()
	{		
	}
}//namespace ngl