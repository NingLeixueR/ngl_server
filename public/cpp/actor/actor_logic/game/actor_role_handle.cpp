#include "actor_role.h"

namespace ngl
{
	bool actor_role::handle(message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata)
	{
		sync_data_client();
		LogLocalError("[sync]###[%]", m_info.get()->getconst().m_base().m_name())
		return true;
	}

	bool actor_role::handle(message<pbnet::PROBUFF_NET_GET_TIME>& adata)
	{
		//auto pro = std::make_shared<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>();
		//pro->set_m_utc(localtime::gettime());
		//send2client(pro);
		std::cout << "######Get Server Time##" << localtime::gettime() << std::endl;
		
		i64_actorid lrequest = adata.m_pack->m_head.get_request_actor();
		std::cout
			<< " type			= " << nguid::type(lrequest)
			<< " actordataid	= " << nguid::actordataid(lrequest)
			<< " area			= " << nguid::area(lrequest)
			<< std::endl;
		//LogLocalError("######Get Server Time##[%][%]", m_info.id(), m_info.db()->name())

		pbnet::PROBUFF_NET_GET_TIME_RESPONSE pro;
		pro.set_m_utc(localtime::gettime());
		sendkcp(pro, adata.m_pack->m_head.get_actordataid());

		//
		return true;
	}

	// PROBUFF_PROTOCOLNUM_LOGIC_SWITCH_LINE		= 18;			// [ÇëÇó]ÇÐ»»ÏßÂ·
	bool actor_role::handle(message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata)
	{
		tab_servers* tab = ttab_servers::node_tnumber(GAME, adata.m_data->m_line());
		if (tab == nullptr)
			return false;
		i32_sessionid lsession = nserver->get_sessionid(tab->m_id);
		if (lsession == -1)
		{
			LogLocalError("LOGIC_SWITCH_LINE Error line[%] severid[%]"
				, adata.m_data->m_line(), tab->m_id
			)
			return false;
		}
		actor_switch_process_role pro;
		pro.m_create = false;
		pro.m_gatewayid = m_gatewayid;
		actor_create::switch_process(id_guid(), nconfig::m_nodeid, tab->m_id, pro);
		//LogLocalError("######Switch Line##[%][%]", m_info.id(), m_info.db()->name())
		//crossprocess<actor_role>(lserverid);
		return true;
	}

	bool actor_role::handle(message<actor_send_item>& adata)
	{
		auto lparm = adata.m_data;
		d_remakes(this, lparm->m_src);
		m_bag.add_item(lparm->m_item);
		return true;
	}

	bool actor_role::handle(message<actor_disconnect_close>& adata)
	{
		erase_actor_byid();
		return true;
	}

	bool actor_role::handle(message<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>& adata)
	{
		m_playactorid = adata.m_data->m_playsactorid();
		return true;
	}
}//namespace ngl