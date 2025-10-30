#include "nprotocol.h"
#include "actor_log.h"

namespace ngl
{
	bool nactor_logitem::m_init = false;

	void nactor_logitem::send(std::shared_ptr<np_logitem> pro, bool aisnet)
	{
		if (aisnet)
		{
			// # ACTOR_TYPE # ELOG_TYPE
			actor::send_actor(actor_log::actorid(ttab_servers::instance().tab()->m_log), nguid::make(), pro);
		}
		else
		{
			actor::send_actor(actor_log::actorid(nconfig::m_tid), nguid::make(), pro);
		}
	}
}//namespace ngl