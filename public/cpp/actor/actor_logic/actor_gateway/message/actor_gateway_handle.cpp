/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements message handlers for message.


#include "actor/actor_logic/actor_gateway_c2g/actor_gateway_c2g.h"
#include "actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h"
#include "actor/actor_logic/actor_gateway/actor_gateway.h"
#include "actor/actor_logic/actor_kcp/actor_kcp.h"
#include "actor/actor_logic/actor_server/actor_server.h"
#include "actor/actor_base/nregister.h"
#include "net/nnet.h"

namespace ngl
{
	bool actor_gateway::handle(const message<np_gateway_close_session>& adata)
	{
		const auto* lrecv = adata.get_data();
		i32_actordataid lroleid = lrecv->m_roleid;
		i16_area larea = lrecv->m_area;
		gateway_socket* linfo = m_info.get(larea, lroleid);
		if (linfo == nullptr)
		{
			return true;
		}

		if (linfo->m_socket == 0)
		{
			m_info.remove_actorid(nguid::make(ACTOR_NONE, larea, lroleid));
		}

		{
			auto pro = std::make_shared<np_actor_gatewayinfo_updata>();
			pro->m_delactorid.emplace_back(nguid::make(ACTOR_NONE, larea, lroleid));
			update_gateway_info(pro);
		}

		{
			auto pro = std::make_shared<np_actor_disconnect_close>();
			pro->m_actorid = nguid::make(ACTOR_ROLE, larea, lroleid);
			// # Notifygameserver player connection
			send_actor(pro->m_actorid, id_guid(), pro);
			// # Notifyloginserver player connection
			ttab_servers::instance().foreach_server(LOGIN, tab_self_area, [&pro, this](const tab_servers* atab)
				{
					nguid lguid(ACTOR_LOGIN, tab_self_area, atab->m_id);
					send_actor(lguid, id_guid(), pro);
				});
		}
		return true;
	}

	bool actor_gateway::handle(const message<np_actorrole_login>& adata)
	{// LoginservernotifyGateWayserver playeraccount successful
		const auto* lparm = adata.get_data();
		nguid lguid(lparm->m_roleid);

		const auto linfo = m_info.get(lguid.area(), lguid.actordataid());
		if (linfo != nullptr)
		{
			return true;
		}
		gateway_socket ltemp
		{
			.m_session = lparm->m_session,
			.m_area = lguid.area(),
			.m_accountid = lparm->m_accountid,
			.m_dataid = lguid.actordataid(),
			.m_gameid = lparm->m_gameid,
			.m_gatewayid = lparm->m_gatewayid,
			.m_socket = 0,
		};
		m_info.updata(ltemp);

		update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(
				np_actor_gatewayinfo_updata
				{ 
					.m_add = {ltemp} 
				}
			)
		);

		// Notifyactor_server [actorid]->[gateway server id]
		sync_actorserver_gatewayid(lguid, false);
		return true;
	}

	bool actor_gateway::handle(const message<np_actor_kcp>& adata)
	{
		const auto* lpram = adata.get_data();
		pbnet::PROBUFF_NET_KCPSESSION_RESPONSE pro;
		pro.set_mkcpsession(lpram->m_kcpsession);
		pro.set_m_kcpnum(lpram->m_kcpnum);
		pro.set_mserverid(lpram->m_serverid);
		pro.set_mactoridserver(lpram->m_actoridserver);
		pro.set_mactoridclient(lpram->m_actoridclient);
		
		nnet::instance().send(lpram->m_sessionid, pro, lpram->m_actoridclient, nguid::make());
		return true;
	}

	bool actor_gateway::handle(const message<np_actorswitch_process<np_actorswitch_process_role>>& adata)
	{
		const auto* lpram = adata.get_data();
		nguid lguid(lpram->m_actor);

		gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
		if (linfo == nullptr)
		{
			tools::no_core_dump();
			return true;
		}

		if (lpram->m_toserverid != 0)
		{
			linfo->m_gameid = lpram->m_toserverid;
			update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(
					np_actor_gatewayinfo_updata
					{ 
						.m_add = {*linfo} 
					}
				)
			);
		}
		return true;
	}

	bool actor_gateway::handle(const message<np_actor_session_close>& adata)
	{
		const auto* lpram = adata.get_data();
		if (sysconfig::robot_test())
		{
			std::vector<gateway_socket*> lvec;
			m_info.foreach([&lvec, lpram](gateway_socket* agetway)
				{
					if (lpram->m_sessionid == agetway->m_socket)
					{
						lvec.emplace_back(agetway);
					}
				}
			);
			for (gateway_socket* asocket : lvec)
			{
				gateway_socket* linfo = m_info.get(asocket->m_area, asocket->m_dataid);
				session_close(linfo);
			}
		}
		else
		{
			gateway_socket* linfo = m_info.get(lpram->m_sessionid);
			if (linfo == nullptr)
			{
				return true;
			}
			session_close(linfo);
		}

		update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(
				np_actor_gatewayinfo_updata
				{ 
					.m_delsocket = {lpram->m_sessionid} 
				}
			)
		);

		m_info.remove_socket(lpram->m_sessionid);
		return true;
	}

	bool actor_gateway::handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		const auto* lpram = adata.get_data();
		const pack* lpack = adata.get_pack();

		log_error()->print("# GateWay Login[{}][{}][{}] #", lpack->m_id, nguid(lpram->mroleid()), lpram->msession());
		nguid lguid(lpram->mroleid());
		gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
		if (linfo == nullptr || linfo->m_session != lpram->msession())
		{
			log_error()->print("# GateWay Login Fail");
			return true;
		}
		
		if (lpack->m_id != linfo->m_socket && linfo->m_socket > 0)
		{
			i32_socket loldsocket = linfo->m_socket;
			nnet::instance().close_net(linfo->m_socket);
			m_info.remove_socket(linfo->m_socket);
			linfo->m_socket = 0;
			if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
			{
				update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata
						{
							.m_add = { *linfo },
							.m_delsocket = { loldsocket },
							.m_delactorid = { nguid::make(ACTOR_ROLE,lguid.area(), lguid.actordataid()) },
						}
					)
				);
			}
			// Or
			i64_actorid lroleactor = nguid::make(ACTOR_ROLE, lguid.area(), lguid.actordataid());
			pbnet::PROBUFF_NET_ROLE_SYNC ltemp;
			ntcp::instance().send_server(linfo->m_gameid, ltemp, lroleactor, nguid::make());
			return true;
		}

		if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
		{
			update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(
					np_actor_gatewayinfo_updata
					{ 
						.m_add = {*linfo} 
					}
				)
			);
		}

		pbnet::PROBUFF_NET_ROLE_LOGIN lprampro = *lpram;
		lprampro.set_mgatewayid(nconfig.nodeid());
		lprampro.set_marea(linfo->m_area);
		ntcp::instance().send_server(linfo->m_gameid, lprampro, nguid::moreactor(), id_guid());
		return true;
	}

	bool actor_gateway::handle(const message<pbnet::PROBUFF_NET_KCPSESSION>& adata)
	{
		const auto* lpram = adata.get_data();
		const pack* lpack = adata.get_pack();

		std::string lkcpsession;
		if (ukcp::session_create(lpram->mactoridserver(), lpram->mactoridclient(), lkcpsession) == false)
		{
			return true;
		}

		// ### Notifykcpservercreateconnection
		auto pro = std::make_shared<np_actor_kcp>();
		pro->m_kcpsession		= lkcpsession;
		pro->m_sessionid		= lpack->m_id;
		pro->m_actoridclient	= lpram->mactoridclient();
		pro->m_actoridserver	= lpram->mactoridserver();
		pro->m_uip				= lpram->muip();
		pro->m_uport			= static_cast<i16_port>(lpram->muport());
		pro->m_conv				= lpram->mconv();
		pro->m_kcpnum			= lpram->m_kcpnum();
		pro->m_serverid			= static_cast<i32_serverid>(lpram->mserverid());

		actor::send_actor(actor_kcp::actorid(pro->m_serverid), id_guid(), pro);
		return true;
	}
}//namespace ngl
