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
#include "actor_robot_manage.h"
#include "nkcp.h"

namespace ngl
{
	actor_robot_manage::actor_robot_manage() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROBOT_MANAGE,
					.m_area = tab_self_area,
					.m_id = nconfig.tid(),
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_robot_manage::actor_type()
	{
		return ACTOR_ROBOT_MANAGE;
	}

	i64_actorid actor_robot_manage::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_robot_manage::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_robot_manage::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_robot_manage::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_robot_manage::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_robot_manage::nregister()
	{
		// 定时器
		actor::register_timer<actor_robot_manage>(&actor_robot_manage::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_robot_manage
			, np_robot_pram
			, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE
			, pbnet::PROBUFF_NET_ROLE_NOT_CREATE
		>(e_ready_all);
	}

	std::shared_ptr<actor_robot> actor_robot_manage::create(i16_area aarea, i32_actordataid aroleid)
	{
		return std::dynamic_pointer_cast<actor_robot>(actor_base::create(ENUM_ACTOR::ACTOR_ROBOT, aarea, aroleid, nullptr));
	}

	void actor_robot_manage::login(const std::string& aaccount, const std::string& apasswold)
	{
		pbnet::PROBUFF_NET_ACOUNT_LOGIN pro;
		pro.set_marea(tab_self_area);
		pro.set_maccount(aaccount);
		pro.set_mpassword(apasswold);
		const tab_servers* tab = ttab_servers::instance().const_tab();
		if (tab == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		
		ntcp::instance().send_server(nnodeid::nodeid(tab->m_login, 1), pro, nguid::moreactor(), instance().id_guid());
	}

	bool actor_robot_manage::check_connect(i32_serverid aserverid)const
	{
		net_works lnets;
		return ttab_servers::instance().tab(nnodeid::tid(aserverid)) != nullptr && ttab_servers::instance().connect(aserverid, lnets);
	}

	void actor_robot_manage::connect(i32_serverid aserverid, const std::function<void(i32_sessionid)>& afun) const
	{
		if (check_connect(aserverid))
		{
			ntcp::instance().connect(aserverid, afun, true, false);
		}
	}

	bool actor_robot_manage::parse_command(std::vector<std::string>& aparm)
	{
		auto ldata = std::make_shared<np_robot_pram>();
		ldata->m_parm.swap(aparm);
		i64_actorid lid = ngl::nguid::make(ACTOR_ROBOT_MANAGE, tab_self_area, nconfig.tid());
		handle_pram lparm = ngl::handle_pram::create<np_robot_pram, false, false>(lid, nguid::moreactor(), ldata);
		actor_manage::instance().push_task_id(lid, lparm);
		return true;
	}

	void actor_robot_manage::create_robot(const std::string& arobotname)const
	{
		ngl::actor_robot_manage::login(arobotname, "123456");
	}

	void actor_robot_manage::create_robots(const std::string& arobotname, int abeg, int aend)const
	{
		for (int i = abeg; i <= aend; ++i)
		{
			std::string lname(arobotname);
			lname += "";
			lname += tools::lexical_cast<std::string>(i);
			ngl::actor_robot_manage::login(lname, "123456");
		}
	}

	void actor_robot_manage::foreach(const std::function<bool(_robot&)>& afun)
	{
		for (std::pair<const std::string, _robot>& item : m_maprobot)
		{
			afun(item.second);
		}
	}

	_robot* actor_robot_manage::get_robot(std::string aacount)
	{
		auto itor = m_maprobot.find(aacount);
		if (itor == m_maprobot.end())
		{
			return nullptr;
		}
		return &itor->second;
	}

	_robot* actor_robot_manage::get_robot(i64_actorid aroleid)
	{
		auto itor = m_maprobotbyactorid.find(aroleid);
		if (itor == m_maprobotbyactorid.end())
		{
			return nullptr;
		}
		return itor->second;

	}

	bool actor_robot_manage::getdata(_robot* arobot)
	{
		if (arobot == nullptr)
		{
			return false;
		}
		return true;
	}


	bool actor_robot_manage::kcp_connect(i64_actorid arobotid, pbnet::ENUM_KCP akcpenum, int16_t aservertid, int16_t atcount, i64_actorid aseractorid)
	{
		int32_t lserverid = nnodeid::nodeid(aservertid, atcount);

		const tab_servers* tabserver = ttab_servers::instance().tab(aservertid);
		if (tabserver == nullptr)
		{
			return false;
		}

		net_works lpstructserver;
		if (!ttab_servers::instance().get_nworks(tabserver->m_type, nconfig.area(), ENET_TCP, atcount, lpstructserver))
		{
			return false;
		}

		i16_port luport = nkcp::instance().kcp_port(aservertid, atcount, akcpenum);
		log_error()->print("kcp connect server[{}:{}] {}@{}", aservertid, atcount, lpstructserver.m_ip, luport);

		// 获取本机uip
		ngl::asio_udp_endpoint lendpoint(asio::ip::address::from_string(lpstructserver.m_ip), luport);
		i64_actorid robotid = nguid::make_type(arobotid, ACTOR_ROLE);
		_robot* lprobot = get_robot(robotid);
		if (lprobot == nullptr || lprobot->m_robot == nullptr)
		{
			return false;
		}
		
		lprobot->m_robot->kcp_setindex(lserverid, akcpenum, nkcp::instance().create_kcp());
		int16_t lindex = lprobot->m_robot->kcp_index(lserverid, akcpenum);
		if (lindex == -1)
		{
			return false;
		}
		auto lpukcp = nkcp::instance().kcp(lprobot->m_robot->kcp_index(lserverid, akcpenum));
		if (lpukcp == nullptr)
		{
			return false;
		}

		lpukcp->sendu_waitrecv(lendpoint, "GetIp", sizeof("GetIp"), [this, &lprobot, lserverid, akcpenum, aseractorid](char* buff, int len)
			{
				log_error()->print("Local GetIp Finish : {}", buff);
				ukcp::m_localuip = buff;
				// 获取kcp-session
				pbnet::PROBUFF_NET_KCPSESSION pro;
				pro.set_mserverid(lserverid);
				pro.set_muip(ukcp::m_localuip);
				pro.set_muport(lprobot->m_robot->kcp_index(lserverid, akcpenum));
				pro.set_mconv(ukcp::m_conv);
				pro.set_mactoridclient(lprobot->m_robot->id_guid());
				pro.set_mactoridserver(aseractorid);
				pro.set_m_kcpnum(akcpenum);
				ntcp::instance().send(lprobot->m_session, pro, nguid::moreactor(), lprobot->m_robot->id_guid());
			}
		);
		return true;
	}


	bool actor_robot_manage::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl