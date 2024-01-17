#include "actor_manage_role.h"
#include "actor_timer.h"
#include "actor_register.h"
#include "actor_create.h"

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
					.m_id = nconfig::m_nodeid
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_manage_role::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_manage_role>(false
			, dregister_fun_handle(actor_manage_role, pbnet::PROBUFF_NET_ROLE_LOGIN)
			);
	}

	bool actor_manage_role::handle(message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		actor_guid lguid(adata.m_data->m_roleid());
		std::cout << "actor_manage_role roleid:" << adata.m_data->m_roleid() << std::endl;
		actor_switch_process_role pro
		{
			.m_create = adata.m_data->m_iscreate(),
			.m_gatewayid = adata.m_data->m_gatewayid(),
		};
		actor_create::switch_process(adata.m_data->m_roleid(), 0, nconfig::m_nodeid, pro);
		return true;
	}
}