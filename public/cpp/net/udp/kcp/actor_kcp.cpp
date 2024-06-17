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
		register_handle_custom<actor_kcp>::func<
			np_actor_kcp
		>(false);
	}

	bool actor_kcp::handle(message<np_actor_kcp>& adata)
	{
		auto lpram = adata.get_data();
		auto lpack = adata.m_pack;
		nets::kcp()->reset_add(lpram->m_conv, lpram->m_uip, lpram->m_uport);

		nets::sendbysession(lpack->m_id, *lpram, nguid::make(), nguid::make());
		return true;
	}
}//namespace ngl