#include "actor_gateway.h"
#include "nregister.h"

namespace ngl
{
	actor_gateway::actor_gateway() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = nconfig::m_nodeid, .m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	actor_gateway::~actor_gateway() {}

	void actor_gateway::nregister()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gateway>(
			false
			, dregister_fun_handle(actor_gateway, np_actorrole_login)
			, (Tfun<actor_gateway, np_actorswitch_process<np_actorswitch_process_role>>) & actor_gateway::handle
			, dregister_fun_handle(actor_gateway, np_actor_session_close)
			, dregister_fun_handle(actor_gateway, np_actorrole_login)	
			, dregister_fun_handle(actor_gateway, np_actor_kcp)
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_gateway>(
			false
			, dregister_fun_handle(actor_gateway, pbnet::PROBUFF_NET_ROLE_LOGIN)
			, dregister_fun_handle(actor_gateway, pbnet::PROBUFF_NET_KCPSESSION)
		);
	}

	void actor_gateway::sync_actorserver_gatewayid(const nguid& aguid, bool aisremove)
	{
		np_actor_gatewayid_updata pro
		{
			.m_isremove = aisremove,
			.m_actorid = nguid::make(ACTOR_ROLE, aguid.area(), aguid.actordataid()),
			.m_gatewayid = ttab_servers::tab()->m_id,
		};

		for (i32_serverid iserverid : ttab_servers::tab()->m_actorserver)
		{
			actor_base::send_server(
				iserverid,
				pro,
				nguid::make(ACTOR_ADDRESS_SERVER, aguid.area(), nguid::none_actordataid()),
				nguid::make()
			);
		}
	}

	void actor_gateway::update_gateway_info(np_actor_gatewayinfo_updata* ap)
	{
		std::shared_ptr<np_actor_gatewayinfo_updata> pro(ap);
		send_actor(nguid::make(ACTOR_GATEWAY_GAME2CLIENT, tab_self_area, id()), pro);
		send_actor(nguid::make(ACTOR_GATEWAY_CLIENT2GAME, tab_self_area, id()), pro);
	}

	void actor_gateway::session_close(gateway_socket* ainfo)
	{
		Assert(ainfo != nullptr);

		ainfo->m_socket = 0;
		i32_actordataid lroleid = ainfo->m_dataid;
		i16_area larea = ainfo->m_area;

		int lwaiting = 0;
		nconfig::m_publicinfo.find("waiting", lwaiting);

		wheel_parm lparm
		{
			.m_ms = lwaiting * 1000,
			.m_intervalms = [lwaiting](int64_t) {return lwaiting * 1000; } ,
			.m_count = 1,
			.m_fun = [this, lroleid, larea](wheel_node* anode)
			{
				gateway_socket* linfo = m_info.get(larea, lroleid);
				if (linfo == nullptr)
					return;

				if (linfo->m_socket == 0)
				{
					m_info.remove_actorid(nguid::make(ACTOR_NONE, larea, lroleid));
				}

				update_gateway_info(new np_actor_gatewayinfo_updata{.m_delactorid = {nguid::make(ACTOR_NONE, larea, lroleid)} });

				{
					std::shared_ptr<np_actor_disconnect_close> pro(new np_actor_disconnect_close
						{
							.m_actorid = nguid::make(ACTOR_ROLE, larea, lroleid),
						});
					// ##### 通知game服务器 玩家已经断开连接
					send_actor(pro->m_actorid, pro);
					// ##### 通知login服务器 玩家已经断开连接
					ttab_servers::foreach_server(LOGIN, [&pro, this](const tab_servers* atab)
						{
							nguid lguid(ACTOR_LOGIN, tab_self_area, atab->m_id);
							send_actor(lguid, pro);
						});
				}

			}
		};
		twheel::wheel().addtimer(lparm);

		sync_actorserver_gatewayid(nguid::make(ACTOR_ROLE, larea, lroleid), true);
	}

	bool actor_gateway::handle(message<np_actorrole_login>& adata)
	{// login服务器通知GateWay服务器 玩家账号验证成功
		auto lparm = adata.m_data;
		nguid lguid(lparm->m_roleid);

		gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
		if (linfo != nullptr)
			return true;
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

		update_gateway_info(new np_actor_gatewayinfo_updata{ .m_add = {ltemp} });

		// ## 通知actor_server [actorid]->[gateway server id]
		sync_actorserver_gatewayid(lguid, false);

		nets::net()->send(adata.m_pack->m_id, *lparm, nguid::make_self(ACTOR_LOGIN), nguid::make());
		return true;
	}

	bool actor_gateway::handle(message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		Try
		{
			auto lpram = adata.m_data;
			auto lpack = adata.m_pack;
			Assert(lpack != nullptr);
			LogLocalError("############ GateWay Login[%][%][%] ############"
				, lpack->m_id
				, lpram->m_roleid()
				, lpram->m_session()
			)
			nguid lguid(lpram->m_roleid());
			gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
			
			Assert(linfo != nullptr);
			Assert(linfo->m_session == lpram->m_session());

			if (rebot_test::is_test == false)
			{
				if (lpack->m_id != linfo->m_socket)
				{
					if (linfo->m_socket > 0)
					{
						nserver->close_net(linfo->m_socket);
						linfo->m_socket = 0;
						if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
						{
							update_gateway_info(new np_actor_gatewayinfo_updata{.m_add = {*linfo} });
						}
						// 断线重连或者其他设备顶号
						pbnet::PROBUFF_NET_ROLE_SYNC pro;
						nserver->send_server(linfo->m_gameid, pro, nguid::make(ACTOR_ROLE, lguid.area(), lguid.actordataid()), id_guid());
						return true;
					}
				}
			}

			if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
			{
				update_gateway_info(new np_actor_gatewayinfo_updata{.m_add = {*linfo} });
			}
			lpram->set_m_iscreate(linfo->m_iscreate);
			linfo->m_iscreate = false;
			lpram->set_m_gatewayid(nconfig::m_nodeid);
			lpram->set_m_area(linfo->m_area);
			nserver->send_server(linfo->m_gameid, *lpram, nguid::moreactor(), id_guid());

			return true;
		}Catch;
		return false;
	}

	bool actor_gateway::handle(message<np_actor_kcp>& adata)
	{
		auto lpram = adata.m_data;
		pbnet::PROBUFF_NET_KCPSESSION_RESPONSE pro;
		pro.set_m_kcpsession(lpram->m_kcpsession);
		nets::net()->send(lpram->m_sessionid, pro,
			nguid::make(ACTOR_ROBOT, lpram->m_area, lpram->m_dataid)
			, nguid::make()
		);
		return true;
	}

	// 获取kcp-session
	bool actor_gateway::handle(message<pbnet::PROBUFF_NET_KCPSESSION>& adata)
	{
		auto lpram = adata.m_data;
		auto lpack = adata.m_pack;
		gateway_socket* lpstruct = m_info.get(lpack->m_id);
		if (lpstruct == nullptr)
			return true;
		
		std::string lkcpsession;
		if (ukcp::create_session(nguid::make(nguid::none_type(), lpstruct->m_area, lpstruct->m_dataid), lkcpsession) == false)
			return true;
		

		// ### 通知kcp服务器创建连接
		np_actor_kcp pro;
		pro.m_kcpsession = lkcpsession;
		pro.m_sessionid = lpack->m_id;
		pro.m_area = lpstruct->m_area;
		pro.m_dataid = lpstruct->m_dataid;
		pro.m_uip = lpram->m_uip();
		pro.m_uport = lpram->m_uport();
		pro.m_conv = lpram->m_conv();
		nets::net()->send_server(lpram->m_serverid(), pro, -1, -1);

		return true;
	}

	bool actor_gateway::handle(message<np_actorswitch_process<np_actorswitch_process_role>>& adata)
	{
		Try
		{
			auto lpram = adata.m_data;
			LogLocalInfo("############ GateWay Transmit ############");
			nguid lguid(lpram->m_actor);

			gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
			Assert(linfo != nullptr);
			if (lpram->m_toserverid != 0)
			{
				linfo->m_gameid = lpram->m_toserverid;
				update_gateway_info(new np_actor_gatewayinfo_updata{.m_add = {*linfo} });
			}
		}Catch;
		return true;
	}

	bool actor_gateway::handle(message<np_actor_session_close>& adata)
	{
		Try
		{
			auto lpram = adata.m_data;
			if (rebot_test::is_test)
			{
				std::vector<gateway_socket*> lvec;
				m_info.foreach([&lvec, lpram](gateway_socket* agetway)
					{
						if (lpram->m_sessionid == agetway->m_socket)
							lvec.push_back(agetway);
					});
				for (gateway_socket* item : lvec)
				{
					gateway_socket* linfo = m_info.get(item->m_area, item->m_dataid);
					session_close(linfo);
					LogLocalInfo("############ earse_roleinfobysocket[%]:[%] ############", nguid::make(ACTOR_ROLE, linfo->m_area, linfo->m_dataid), lpram->m_sessionid);
				}
			}
			else
			{
				gateway_socket* linfo = m_info.get(lpram->m_sessionid);
				Assert(linfo != nullptr);
				session_close(linfo);
				LogLocalInfo("############ earse_roleinfobysocket[%]:[%] ############", nguid::make(ACTOR_ROLE, linfo->m_area, linfo->m_dataid), lpram->m_sessionid);
			}

			update_gateway_info(new np_actor_gatewayinfo_updata{.m_delsocket = {lpram->m_sessionid} });

			m_info.remove_socket(lpram->m_sessionid);
			return true;
		}Catch;
		return false;
	}
}//namespace ngl