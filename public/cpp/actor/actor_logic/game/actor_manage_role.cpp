#include "actor_manage_role.h"
#include "actor_create.h"
#include "nregister.h"
#include "ntimer.h"

namespace ngl
{
	actor_manage_role::actor_manage_role() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MANAGE_ROLE,
					.m_area = ttab_servers::tab()->m_area,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_manage_role::nregister()
	{
		register_handle_proto<actor_manage_role>::func<
			pbnet::PROBUFF_NET_ROLE_LOGIN
		>(false);
	}

	i64_actorid actor_manage_role::actorid()
	{
		return nguid::make(ACTOR_MANAGE_ROLE, ttab_servers::tab()->m_area, nguid::none_actordataid());
	}

	bool actor_manage_role::handle(message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		nguid lguid(adata.get_data()->m_roleid());
		log_error()->print("actor_manage_role roleid:{}", adata.get_data()->m_roleid());
		np_actorswitch_process_role pro
		{
			.m_create = adata.get_data()->m_iscreate(),
			.m_gatewayid = adata.get_data()->m_gatewayid(),
		};
		actor_create::switch_process(adata.get_data()->m_roleid(), 0, nconfig::m_nodeid, pro);
		return true;
	}
}//namespace ngl