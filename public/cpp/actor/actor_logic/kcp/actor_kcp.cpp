#include "actor_kcp.h"
#include "ukcp.h"


namespace ngl
{
	actor_kcp::actor_kcp():
		actor(
		actorparm
		{
			.m_parm
			{
				.m_type = ACTOR_KCP,
				.m_area = ttab_servers::tab()->m_area,
				.m_id = nconfig::m_nodeid,
			},
			.m_weight = 0x7fffffff
		})
	{

	}

	void actor_kcp::nregister()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_kcp>(
			false
			, dregister_fun_handle(actor_kcp, actor_protocol_kcp)
		);
	}

	bool actor_kcp::handle(message<actor_protocol_kcp>& adata)
	{
		auto lpram = adata.m_data;
		auto lpack = adata.m_pack;
		ukcp::getInstance().reset_add(lpram->m_conv, lpram->m_uip, lpram->m_uport);

		nets::net()->send(lpack->m_id, *lpram, -1, -1);
		return true;
	}
}