#include "nprotocol.h"
#include "actor_log.h"

namespace ngl
{
	bool nactor_logitem::m_init = false;

	void nactor_logitem::send(std::shared_ptr<np_logitem> pro)
	{
		// # ACTOR_TYPE # ELOG_TYPE
		actor::static_send_actor(actor_log::actorid(m_actortype, m_logtype), nguid::make(), pro);
	}
}//namespace ngl