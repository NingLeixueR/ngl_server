#include "actor_manage_role.h"
#include "actor_timer.h"
#include "actor_register.h"
#include "actor_switchprocess.h"

namespace ngl
{
	void actor_manage_role::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_manage_role>(false,
				null<pbnet::PROBUFF_NET_ROLE_LOGIN>
			);
		// ----¶¨Ê±Æ÷----
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