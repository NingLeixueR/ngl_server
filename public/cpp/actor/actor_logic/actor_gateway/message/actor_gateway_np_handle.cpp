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
}//namespace ngl