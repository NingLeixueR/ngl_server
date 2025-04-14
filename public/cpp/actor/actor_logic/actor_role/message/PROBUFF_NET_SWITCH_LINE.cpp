#include "actor_role.h"
#include "ttab_task.h"
#include "drop.h"

namespace ngl
{
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata)
	{
		const tab_servers* tab = ttab_servers::node_tnumber(GAME, adata.get_data()->m_line());
		if (tab == nullptr)
		{
			return false;
		}
		i32_sessionid lsession = server_session::sessionid(tab->m_id);
		if (lsession == -1)
		{
			log_error()->print("PROBUFF_NET_SWITCH_LINE ERROR line[{}] severid[{}]", adata.get_data()->m_line(), tab->m_id);
			return false;
		}
		np_actorswitch_process_role pro;
		pro.m_create = false;
		pro.m_gatewayid = m_gatewayid;
		actor_create::switch_process(id_guid(), nconfig::m_nodeid, tab->m_id, pro);
		return true;
	}
}//namespace ngl