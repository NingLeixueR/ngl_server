/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_kcp.h"
#include "nkcp.h"
namespace ngl
{
	bool actor_kcp::handle(const message<np_actor_kcp>& adata)
	{
		auto lpram = adata.get_shared_data();
		auto lpack = adata.get_pack();
		
		if (lpram->m_actoridserver != nguid::make() && !actor_manage::instance().is_have_actor(lpram->m_actoridserver))
		{
			log_error()->print("np_actor_kcp fail actor:{}",nguid(lpram->m_actoridserver));
			return true;
		}
		
		ukcp* lukcp = nkcp::instance().serkcp(lpram->m_kcpnum, nconfig.tcount());
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