#include "actor_manage_role.h"
#include "actor_timer.h"
#include "actor_register.h"
#include "actor_switchprocess.h"

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
		// ----��ʱ��----
		//actor_manage_role::register_timer<actor_manage_role>();
	}

	

	bool actor_manage_role::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
	{
		actor_guid lguid(adata.m_roleid());
		LogLocalError("actor_manage_role::handle LOGIC_ROLE_LOGIN [%]", lguid);
		actor_switch_process_role pro
		{
			.m_create = adata.m_iscreate(),
			.m_gatewayid = adata.m_gatewayid(),
		};
		actor_switchprocess::switch_process(adata.m_roleid(), 0, nconfig::m_nodeid, pro);
		return true;
	}



}