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
			// ##### 通知game服务器 玩家已经断开连接
			send_actor(pro->m_actorid, pro);
			// ##### 通知login服务器 玩家已经断开连接
			ttab_servers::foreach_server(LOGIN, tab_self_area, [&pro, this](const tab_servers* atab)
				{
					nguid lguid(ACTOR_LOGIN, tab_self_area, atab->m_id);
					send_actor(lguid, pro);
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

		update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata{ .m_add = {ltemp} }));

		// ## 通知actor_server [actorid]->[gateway server id]
		sync_actorserver_gatewayid(lguid, false);

		return true;
	}

	bool actor_gateway::handle(const message<np_actor_kcp>& adata)
	{
		auto lpram = adata.get_data();
		pbnet::PROBUFF_NET_KCPSESSION_RESPONSE pro;
		pro.set_m_kcpsession(lpram->m_kcpsession);
		nets::sendbysession(lpram->m_sessionid, pro, nguid::make(ACTOR_ROBOT, lpram->m_area, lpram->m_dataid), nguid::make());
		return true;
	}

	bool actor_gateway::handle(const message<np_actorswitch_process<np_actorswitch_process_role>>& adata)
	{
		Try
		{
			auto lpram = adata.get_data();
			nguid lguid(lpram->m_actor);

			gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
			Assert(linfo != nullptr);
			if (lpram->m_toserverid != 0)
			{
				linfo->m_gameid = lpram->m_toserverid;
				update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata{.m_add = {*linfo} }));
			}
		}Catch
			return true;
	}

	bool actor_gateway::handle(const message<np_actor_session_close>& adata)
	{
		Try
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
				std::ranges::for_each(lvec, [this](gateway_socket* asocket)
					{
						gateway_socket* linfo = m_info.get(asocket->m_area, asocket->m_dataid);
						session_close(linfo);
					});
			}
			else
			{
				gateway_socket* linfo = m_info.get(lpram->m_sessionid);
				Assert(linfo != nullptr);
				session_close(linfo);
			}

			update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata{.m_delsocket = {lpram->m_sessionid}}));

			m_info.remove_socket(lpram->m_sessionid);
			return true;
		}Catch
			return false;
	}

	bool actor_gateway::handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		Try
		{
			auto lpram = adata.get_data();
			auto lpack = adata.get_pack();
			Assert(lpack != nullptr);
			log_error()->print("############ GateWay Login[{}][{}][{}] ############"
				, lpack->m_id, lpram->m_roleid(), lpram->m_session()
			);
			nguid lguid(lpram->m_roleid());
			gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());

			Assert(linfo != nullptr);
			Assert(linfo->m_session == lpram->m_session());

			if (sysconfig::robot_test() == false && lpack->m_id != linfo->m_socket && linfo->m_socket > 0)
			{
				nets::net(linfo->m_socket)->close_net(linfo->m_socket);
				linfo->m_socket = 0;
				if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
				{
					update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata{.m_add = {*linfo} }));
				}
				// 断线重连或者其他设备顶号
				pbnet::PROBUFF_NET_ROLE_SYNC pro;
				i64_actorid lroleactor = nguid::make(ACTOR_ROLE, lguid.area(), lguid.actordataid());
				nets::sendbyserver(linfo->m_gameid, pro, lroleactor, id_guid());
				return true;
			}

			if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
			{
				update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata{.m_add = {*linfo} }));
			}

			pbnet::PROBUFF_NET_ROLE_LOGIN lprampro = *lpram;
			lprampro.set_m_iscreate(linfo->m_iscreate);
			linfo->m_iscreate = false;
			lprampro.set_m_gatewayid(nconfig::m_nodeid);
			lprampro.set_m_area(linfo->m_area);
			nets::sendbyserver(linfo->m_gameid, *lpram, nguid::moreactor(), id_guid());
			return true;
		}Catch
		return false;
	}

	bool actor_gateway::handle(const message<pbnet::PROBUFF_NET_KCPSESSION>& adata)
	{
		auto lpram = adata.get_data();
		auto lpack = adata.get_pack();

		/////////////////////////////////////
		// 多robot公用一个tcp连接会有问题
		//gateway_socket* lpstruct = m_info.get(lpack->m_id);
		//if (lpstruct == nullptr)
		//	return true;
		//i16_area larea = lpstruct->m_area;
		//i32_actordataid lactordataid = lpstruct->m_dataid;
		i64_actorid request_actor = lpack->m_head.get_request_actor();
		i16_area larea = nguid::area(request_actor);
		i32_actordataid lactordataid = nguid::actordataid(request_actor);
		///////////////////////////////

		std::string lkcpsession;
		if (ukcp::create_session(nguid::make(nguid::none_type(), larea, lactordataid), lkcpsession) == false)
		{
			return true;
		}

		// ### 通知kcp服务器创建连接
		np_actor_kcp pro;
		pro.m_kcpsession = lkcpsession;
		pro.m_sessionid = lpack->m_id;
		pro.m_area = nguid::area(request_actor);
		pro.m_dataid = nguid::actordataid(request_actor);
		pro.m_uip = lpram->m_uip();
		pro.m_uport = lpram->m_uport();
		pro.m_conv = lpram->m_conv();

		nets::sendbyserver((i32_serverid)lpram->m_serverid(), pro, nguid::make(), nguid::make());

		return true;
	}
}//namespace ngl