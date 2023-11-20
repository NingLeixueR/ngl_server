#include "actor_gateway.h"
#include "actor_register.h"

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

	void actor_gateway::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gateway>(
			false
			, dregister_fun_handle(actor_gateway, actor_role_login)
			, (Tfun<actor_gateway, actor_switch_process<actor_switch_process_role>>) & actor_gateway::handle
			, dregister_fun_handle(actor_gateway, actor_session_close)
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_gateway>(
			false
			, dregister_fun_handle(actor_gateway, pbnet::PROBUFF_NET_ROLE_LOGIN)
		);
	}

	void actor_gateway::sync_actorserver_gatewayid(const actor_guid& aguid, bool aisremove)
	{
		actor_gateway_id_updata pro
		{
			.m_isremove = aisremove,
			.m_actorid = actor_guid::make(ACTOR_ROLE, aguid.area(), aguid.actordataid()),
			.m_gatewayid = ttab_servers::tab()->m_id,
		};

		for (i32_serverid iserverid : ttab_servers::tab()->m_actorserver)
		{
			actor_base::sendtoserver(
				iserverid,
				pro,
				actor_guid::make(ACTOR_ADDRESS_SERVER, aguid.area(), actor_guid::none_actordataid()),
				actor_guid::make()
			);
		}
	}

	void actor_gateway::update_gateway_info(actor_gateway_info_updata* ap)
	{
		std::shared_ptr<actor_gateway_info_updata> pro(ap);
		send_actor(actor_guid::make(ACTOR_GATEWAY_GAME2CLIENT, tab_self_area, id()), pro);
		send_actor(actor_guid::make(ACTOR_GATEWAY_CLIENT2GAME, tab_self_area, id()), pro);
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
					m_info.remove_actorid(actor_guid::make(ACTOR_NONE, larea, lroleid));
				}

				update_gateway_info(new actor_gateway_info_updata{.m_delactorid = {actor_guid::make(ACTOR_NONE, larea, lroleid)} });

				{
					std::shared_ptr<actor_disconnect_close> pro(new actor_disconnect_close
						{
							.m_actorid = actor_guid::make(ACTOR_ROLE, larea, lroleid),
						});
					// ##### 通知game服务器 玩家已经断开连接
					send_actor(pro->m_actorid, pro);
					// ##### 通知login服务器 玩家已经断开连接
					ttab_servers::foreach_server(LOGIN, [&pro, this](const tab_servers* atab)
						{
							actor_guid lguid(ACTOR_LOGIN, tab_self_area, atab->m_id);
							send_actor(lguid, pro);
						});
				}

			}
		};
		twheel::wheel().addtimer(lparm);

		sync_actorserver_gatewayid(actor_guid::make(ACTOR_ROLE, larea, lroleid), true);
	}

	bool actor_gateway::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_role_login& adata)
	{// login服务器通知GateWay服务器 玩家账号验证成功
		actor_guid lguid(adata.m_roleid);

		gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
		if (linfo != nullptr)
			return true;
		gateway_socket ltemp
		{
				.m_session = adata.m_session,
				.m_area = lguid.area(),
				.m_accountid = adata.m_accountid,
				.m_dataid = lguid.actordataid(),
				.m_gameid = adata.m_gameid,
				.m_gatewayid = adata.m_gatewayid,
				.m_socket = 0,
				.m_iscreate = adata.m_iscreate,
		};
		m_info.updata(ltemp);

		update_gateway_info(new actor_gateway_info_updata{ .m_add = {ltemp} });

		// ## 通知actor_server [actorid]->[gateway server id]
		sync_actorserver_gatewayid(lguid, false);

		return true;
	}

	bool actor_gateway::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ROLE_LOGIN& adata)
	{
		Try
		{
			Assert(apack != nullptr);
			LogLocalError("############ GateWay Login[%][%][%] ############", apack->m_id, adata.m_roleid(), adata.m_session());
			actor_guid lguid(adata.m_roleid());
			gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
			Assert(linfo != nullptr);
			Assert(linfo->m_session == adata.m_session());

			if (rebot_test::is_test == false)
			{
				if (apack->m_id != linfo->m_socket)
				{
					if (linfo->m_socket > 0)
					{
						nserver->close_net(linfo->m_socket);
						linfo->m_socket = 0;
						if (m_info.updata_socket(lguid.area(), lguid.actordataid(), apack->m_id))
						{
							update_gateway_info(new actor_gateway_info_updata{.m_add = {*linfo} });
						}
						// 断线重连或者其他设备顶号
						pbnet::PROBUFF_NET_ROLE_SYNC pro;
						nserver->sendtoserver(linfo->m_gameid, pro, actor_guid::make(ACTOR_ROLE, lguid.area(), lguid.actordataid()), id_guid());
						return true;
					}
				}
			}

			if (m_info.updata_socket(lguid.area(), lguid.actordataid(), apack->m_id))
			{
				update_gateway_info(new actor_gateway_info_updata{.m_add = {*linfo} });
			}
			adata.set_m_iscreate(linfo->m_iscreate);
			linfo->m_iscreate = false;
			adata.set_m_gatewayid(nconfig::m_nodeid);
			adata.set_m_area(linfo->m_area);
			nserver->sendtoserver(linfo->m_gameid, adata, actor_guid::moreactor(), id_guid());

			return true;
		}Catch;
		return false;
	}

	bool actor_gateway::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_switch_process<actor_switch_process_role>& adata)
	{
		Try
		{
			LogLocalInfo("############ GateWay Transmit ############");
			actor_guid lguid(adata.m_actor);

			gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());
			Assert(linfo != nullptr);
			if (adata.m_toserverid != 0)
			{
				linfo->m_gameid = adata.m_toserverid;
				update_gateway_info(new actor_gateway_info_updata{.m_add = {*linfo} });
			}
		}Catch;
		return true;
	}

	bool actor_gateway::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_session_close& adata)
	{
		Try
		{
			if (rebot_test::is_test)
			{
				std::vector<gateway_socket*> lvec;
				m_info.foreach([&lvec, &adata](gateway_socket* agetway)
					{
						if (adata.m_sessionid == agetway->m_socket)
							lvec.push_back(agetway);
					});
				for (gateway_socket* item : lvec)
				{
					gateway_socket* linfo = m_info.get(item->m_area, item->m_dataid);
					session_close(linfo);
					LogLocalInfo("############ earse_roleinfobysocket[%]:[%] ############", actor_guid::make(ACTOR_ROLE, linfo->m_area, linfo->m_dataid), adata.m_sessionid);
				}
			}
			else
			{
				gateway_socket* linfo = m_info.get(adata.m_sessionid);
				Assert(linfo != nullptr);
				session_close(linfo);
				LogLocalInfo("############ earse_roleinfobysocket[%]:[%] ############", actor_guid::make(ACTOR_ROLE, linfo->m_area, linfo->m_dataid), adata.m_sessionid);
			}

			update_gateway_info(new actor_gateway_info_updata{.m_delsocket = {adata.m_sessionid} });

			m_info.remove_socket(adata.m_sessionid);
			return true;
		}Catch;
		return false;
	}
}