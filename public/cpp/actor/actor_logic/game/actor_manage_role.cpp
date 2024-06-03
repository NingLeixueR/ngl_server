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
		type_register_actor_handle<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_manage_role>::func<
			pbnet::PROBUFF_NET_ROLE_LOGIN
		>(false);
	}

	i64_actorid actor_manage_role::actorid()
	{
		return nguid::make(ACTOR_MANAGE_ROLE, ttab_servers::tab()->m_area, nguid::none_actordataid());
	}

	bool actor_manage_role::handle(message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		nguid lguid(adata.m_data->m_roleid());
		log_error()->print("actor_manage_role roleid:{}", adata.m_data->m_roleid());
		np_actorswitch_process_role pro
		{
			.m_create = adata.m_data->m_iscreate(),
			.m_gatewayid = adata.m_data->m_gatewayid(),
		};
		actor_create::switch_process(adata.m_data->m_roleid(), 0, nconfig::m_nodeid, pro);
		return true;
	}
}//namespace ngl