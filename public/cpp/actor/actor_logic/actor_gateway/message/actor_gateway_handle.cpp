/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_gateway_c2g.h"
#include "actor_gateway_g2c.h"
#include "actor_gateway.h"
#include "actor_server.h"
#include "nregister.h"

namespace ngl
{
	bool actor_gateway::handle(const message<np_gateway_close_session>& adata)
	{
		i32_actordataid lroleid = adata.get_data()->m_roleid;
		i16_area larea = adata.get_data()->m_area;
		gateway_socket* linfo = m_info.get(larea, lroleid);
		if (linfo == nullptr)
		{
			return true;
		}

		if (linfo->m_socket == 0)
		{
			m_info.remove_actorid(nguid::make(ACTOR_NONE, larea, lroleid));
		}

		auto pro = std::make_shared<np_actor_gatewayinfo_updata>();
		pro->m_delactorid.push_back(nguid::make(ACTOR_NONE, larea, lroleid));
		update_gateway_info(pro);

		{
			auto pro = std::make_shared<np_actor_disconnect_close>();
			pro->m_actorid = nguid::make(ACTOR_ROLE, larea, lroleid);
			// # 通知game服务器 玩家已经断开连接
			send_actor(pro->m_actorid, id_guid(), pro);
			// # 通知login服务器 玩家已经断开连接
			ttab_servers::instance().foreach_server(LOGIN, tab_self_area, [&pro, this](const tab_servers* atab)
				{
					nguid lguid(ACTOR_LOGIN, tab_self_area, atab->m_id);
					send_actor(lguid, id_guid(), pro);
				});
		}
		return true;
	}

	bool actor_gateway::handle(const message<np_actorrole_login>& adata)
	{// login服务器通知GateWay服务器 玩家账号验证成功
		auto lparm = adata.get_data();
		nguid lguid(lparm->m_roleid);

		if (const auto linfo = m_info.get(lguid.area(), lguid.actordataid()); linfo != nullptr)
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
				.m_iscreate = lparm->m_iscreate,
		};
		m_info.updata(ltemp);

		update_gateway_info(
			std::make_shared<np_actor_gatewayinfo_updata>(
				np_actor_gatewayinfo_updata
				{ 
					.m_add = {ltemp} 
				}
			)
		);

		// ## 通知actor_server [actorid]->[gateway server id]
		sync_actorserver_gatewayid(lguid, false);
		return true;
	}

	bool actor_gateway::handle(const message<np_actor_kcp>& adata)
	{
		auto lpram = adata.get_data();
		pbnet::PROBUFF_NET_KCPSESSION_RESPONSE pro;
		pro.set_mkcpsession(lpram->m_kcpsession);
		nets::sendbysession(lpram->m_sessionid, pro, lpram->m_actoridclient, nguid::make());
		return true;
	}

	bool actor_gateway::handle(const message<np_actorswitch_process<np_actorswitch_process_role>>& adata)
	{
		auto lpram = adata.get_data();
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
			update_gateway_info(
				std::make_shared<np_actor_gatewayinfo_updata>(
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
		auto lpram = adata.get_data();
		if (sysconfig::robot_test())
		{
			std::vector<gateway_socket*> lvec;
			m_info.foreach([&lvec, lpram](gateway_socket* agetway)
				{
					if (lpram->m_sessionid == agetway->m_socket)
					{
						lvec.push_back(agetway);
					}
				});
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

		update_gateway_info(
			std::make_shared<np_actor_gatewayinfo_updata>(
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
		auto lpram = adata.get_data();
		auto lpack = adata.get_pack();

		log_error()->print("# GateWay Login[{}][{}][{}] #"
			, lpack->m_id, nguid(lpram->mroleid()), lpram->msession()
		);
		nguid lguid(lpram->mroleid());
		gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
		if (linfo == nullptr || linfo->m_session != lpram->msession())
		{
			//tools::no_core_dump();
			return true;
		}
		
		if (lpack->m_id != linfo->m_socket && linfo->m_socket > 0)
		{
			i32_socket loldsocket = linfo->m_socket;
			nets::net(linfo->m_socket)->close_net(linfo->m_socket);
			m_info.remove_socket(linfo->m_socket);
			linfo->m_socket = 0;
			if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
			{
				update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata
					{
						.m_add = { *linfo },
						.m_delsocket = { loldsocket },
						.m_delactorid = { nguid::make(ACTOR_ROLE,lguid.area(), lguid.actordataid()) },
					}));
			}
			// 断线重连或者其他设备顶号
			i64_actorid lroleactor = nguid::make(ACTOR_ROLE, lguid.area(), lguid.actordataid());
			pbnet::PROBUFF_NET_ROLE_SYNC ltemp;
			nets::sendbyserver(linfo->m_gameid, ltemp, lroleactor, nguid::make());
			return true;
		}

		if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
		{
			update_gateway_info(
				std::make_shared<np_actor_gatewayinfo_updata>(
					np_actor_gatewayinfo_updata
					{ 
						.m_add = {*linfo} 
					}
				)
			);
		}

		pbnet::PROBUFF_NET_ROLE_LOGIN lprampro = *lpram;
		lprampro.set_miscreate(linfo->m_iscreate);
		linfo->m_iscreate = false;
		lprampro.set_mgatewayid(nconfig::m_nodeid);
		lprampro.set_marea(linfo->m_area);
		nets::sendbyserver(linfo->m_gameid, lprampro, nguid::moreactor(), id_guid());
		return true;
	}

	bool actor_gateway::handle(const message<pbnet::PROBUFF_NET_KCPSESSION>& adata)
	{
		auto lpram = adata.get_data();
		auto lpack = adata.get_pack();

		i16_area larea = nguid::none_area();
		i32_actordataid lactordataid = nguid::none_actordataid();
		int32_t lgametid = 0;
		// 多robot公用一个tcp连接会有问题
		if (sysconfig::robot_test())
		{
			i64_actorid request_actor = lpack->m_head.get_request_actor();
			larea = nguid::area(request_actor);
			lactordataid = nguid::actordataid(request_actor);
			gateway_socket* lpstruct = m_info.get(lpack->m_id);
			if (lpstruct == nullptr)
			{
				return true;
			}
			lgametid = lpstruct->m_gameid;
		}
		else
		{
			gateway_socket* lpstruct = m_info.get(lpack->m_id);
			if (lpstruct == nullptr)
			{
				return true;
			}
			larea = lpstruct->m_area;
			lactordataid = lpstruct->m_dataid;
			lgametid = lpstruct->m_gameid;
		}

		std::string lkcpsession;
		if (ukcp::create_session(lpram->mactoridserver(), lpram->mactoridclient(), lkcpsession) == false)
		{
			return true;
		}

		// ### 通知kcp服务器创建连接
		np_actor_kcp pro;
		pro.m_kcpsession	= lkcpsession;
		pro.m_sessionid		= lpack->m_id;
		pro.m_actoridclient = lpram->mactoridclient();
		pro.m_actoridserver = lpram->mactoridserver();
		pro.m_uip			= lpram->muip();
		pro.m_uport			= lpram->muport();
		pro.m_conv			= lpram->mconv();


		nets::sendbyserver((i32_serverid)lgametid, pro, nguid::make(), nguid::make());
		return true;
	}
}//namespace ngl