#include "actor_role.h"


namespace ngl
{
	bool actor_role::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ROLE_SYNC& adata)
	{
		sync_data_client();
		LogLocalError("[sync]###[%]", m_info.get()->getconst().m_base().m_name());
		return true;
	}

	bool actor_role::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_GET_TIME& adata)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>();
		pro->set_m_utc(localtime::gettime());
		send2client(pro);
		//LogLocalError("######Get Server Time##[%][%]", m_info.id(), m_info.db()->name());
		return true;
	}

	// PROBUFF_PROTOCOLNUM_LOGIC_SWITCH_LINE		= 18;			// [ÇëÇó]ÇÐ»»ÏßÂ·
	bool actor_role::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_SWITCH_LINE& adata)
	{
		tab_servers* tab = ttab_servers::node_tnumber(GAME, adata.m_line());
		if (tab == nullptr)
			return false;
		i32_sessionid lsession = nserver->get_sessionid(tab->m_id);
		if (lsession == -1)
		{
			LogLocalError("LOGIC_SWITCH_LINE Error line[%] severid[%]", adata.m_line(), tab->m_id);
			return false;
		}
		actor_switch_process_role pro;
		pro.m_create = false;
		pro.m_gatewayid = m_gatewayid;
		actor_create::switch_process(id_guid(), nconfig::m_nodeid, tab->m_id, pro);
		//LogLocalError("######Switch Line##[%][%]", m_info.id(), m_info.db()->name());
		//crossprocess<actor_role>(lserverid);
		return true;
	}

	bool actor_role::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_send_item& adata)
	{
		d_remakes(this, adata.m_src);
		m_bag.add_item(adata.m_item);
		return true;
	}

	bool actor_role::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_disconnect_close& adata)
	{
		erase_actor_byid();
		return true;
	}

	bool actor_role::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE& adata)
	{
		m_playactorid = adata.m_playsactorid();
		return true;
	}

}