#include "actor_manage_role.h"
#include "actor_keyvalue.h"
#include "actor_create.h"
#include "nregister.h"
#include "ntimer.h"

namespace ngl
{
	bool actor_manage_role::handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		auto recv = adata.get_data();

		if (m_roleban.contains(recv->m_roleid()))
		{
			return true;
		}

		nguid lguid(recv->m_roleid());
		log_error()->print("actor_manage_role roleid:{}", recv->m_roleid());
		np_actorswitch_process_role pro
		{
			.m_create = recv->m_iscreate(),
			.m_gatewayid = recv->m_gatewayid(),
		};
		actor_create::switch_process(recv->m_roleid(), 0, nconfig::m_nodeid, pro);
		return true;
	}
}//namespace ngl