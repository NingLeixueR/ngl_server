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
// File overview: Implements message handlers for message.
#include "actor/actor_logic/actor_kcp/actor_kcp.h"
#include "net/udp/kcp/nkcp.h"
namespace ngl
{
	bool actor_kcp::handle(const message<np_actor_kcp>& adata)
	{
		const auto& lpram = adata.get_shared_data();
		const pack* lpack = adata.get_pack();
		
		if (lpram->m_actoridserver != nguid::make() && !actor_manage::instance().is_have_actor(lpram->m_actoridserver))
		{
			log_error()->print("np_actor_kcp fail actor:{}",nguid(lpram->m_actoridserver));
			return true;
		}
		
		auto lukcp = nkcp::instance().serkcp(lpram->m_kcpnum, nconfig.tcount());
		if (lukcp == nullptr)
		{
			tools::no_core_dump();
			return false;
		}
		lukcp->reset_add(lpram->m_conv, lpram->m_uip, lpram->m_uport, lpram->m_actoridserver, lpram->m_actoridclient);

		if (lpack == nullptr)
		{
			actor::send_actor(nguid::make(ACTOR_GATEWAY, nconfig.area(), nconfig.tcount()), id_guid(), lpram);
		}
		else
		{
			ntcp::instance().send(lpack->m_id, *lpram, nguid::make(), nguid::make());
		}
		return true;
	}
}//namespace ngl
