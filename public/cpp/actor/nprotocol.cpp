#include "nprotocol.h"
#include "actor_log.h"

namespace ngl
{
	bool np_actor_logitem::m_init = false;

	void np_actor_logitem::send(std::shared_ptr<np_actor_logitem> pro)
	{
		// # ACTOR_TYPE # ELOG_TYPE
		actor_base::static_send_actor(actor_log::actorid(m_actortype, m_logtype), nguid::make(), pro);
	}
}