/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for protocol.


#include "actor/protocol/nprotocol.h"
#include "actor/actor_logic/actor_log/actor_log.h"

namespace ngl
{
	bool nactor_logitem::m_init = false;

	void nactor_logitem::send(std::shared_ptr<np_logitem> pro, bool aisnet)
	{
		if (aisnet)
		{
			// # ACTOR_TYPE # ELOG_TYPE
			actor::send_actor(actor_log::actorid(ttab_servers::instance().const_tab()->m_log), nguid::make(), pro);
		}
		else
		{
			actor::send_actor(actor_log::actorid(nconfig.tid()), nguid::make(), pro);
		}
	}
}//namespace ngl