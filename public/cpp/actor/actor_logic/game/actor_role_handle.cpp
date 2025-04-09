#include "actor_role.h"
#include "ttab_task.h"
#include "drop.h"

namespace ngl
{
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata)
	{
		sync_data_client();
		log_error()->print("[sync]###[{}]", m_info.name());
		return true;
	}

	bool actor_role::handle(const message<pbnet::PROBUFF_NET_GET_TIME>& adata)
	{
		i64_actorid lrequest = adata.m_pack->m_head.get_request_actor();
		log_error()->print("{},NAME={}", guid(),m_info.get_constrole().m_base().m_name());
		if (adata.m_pack->m_protocol == ENET_KCP)
		{
			pbnet::PROBUFF_NET_GET_TIME_RESPONSE pro;
			pro.set_m_utc((int32_t)localtime::gettime());
			sendkcp(pro, lrequest);
		}
		else
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>();
			pro->set_m_utc((int32_t)localtime::gettime());
			send_client(id_guid(), pro);
		}
		return true;
	}

	// PROBUFF_PROTOCOLNUM_LOGIC_SWITCH_LINE		= 18;			// [ÇëÇó]ÇÐ»»ÏßÂ·
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

	bool actor_role::handle(const message<np_actor_senditem>& adata)
	{
		auto lparm = adata.get_data();
		local_remakes(this, lparm->m_src);
		m_bag.add_item(lparm->m_item);
		return true;
	}

	bool actor_role::handle(const message<np_actor_disconnect_close>& adata)
	{
		erase_actor_byid();
		return true;
	}

	bool actor_role::handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>& adata)
	{
		tab_task* tab = ttab_task::tab(adata.get_data()->m_taskid());
		if (tab == nullptr)
		{
			return true;
		}

		auto pro = std::make_shared<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>();
		pro->set_m_taskid(adata.get_data()->m_taskid());
		
		std::map<int, int> ldrop;
		if (drop::droplist(tab->m_dropid, 1, ldrop) == false)
		{
			log_error()->print("task:{} drop:{} fail!!!", adata.get_data()->m_taskid(), tab->m_dropid);
			return true;
		}
		tools::copy(ldrop, *pro->mutable_m_drop());
		m_bag.add_item(ldrop);
		return true;
	}
}//namespace ngl