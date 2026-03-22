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
// File overview: Implements logic for actor robot manage.


#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "net/tcp/ws/nws.h"
#include "net/udp/kcp/nkcp.h"

namespace ngl
{
	namespace
	{
		ENET_PROTOCOL normalize_robot_protocol(ENET_PROTOCOL aprotocol)
		{
			return aprotocol == ENET_WS ? ENET_WS : ENET_TCP;
		}

		bool robot_gateway_use_tls()
		{
			const xarg_wss& lwss = nconfig.wss();
			return !lwss.m_certificate_chain.empty() ||
				!lwss.m_private_key.empty() ||
				!lwss.m_ca_certificates.empty() ||
				lwss.m_verify_peer != 1;
		}

		ws_tls_options robot_gateway_tls_options()
		{
			const xarg_wss& lwss = nconfig.wss();
			return ws_tls_options
			{
				.m_certificate_chain = lwss.m_certificate_chain,
				.m_private_key = lwss.m_private_key,
				.m_ca_certificates = lwss.m_ca_certificates,
				.m_verify_peer = lwss.m_verify_peer != 0,
			};
		}

	}

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
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
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
		// Timer.
		actor::register_timer<actor_robot_manage>(&actor_robot_manage::timer_handle);

		// Bind custom np_ messages.
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

	void actor_robot_manage::login(const std::string& aaccount, const std::string& apasswold, ENET_PROTOCOL aprotocol)
	{
		_robot& lrobot = instance().m_maprobot[aaccount];
		lrobot.m_account = aaccount;
		lrobot.m_protocol = normalize_robot_protocol(aprotocol);

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
		
			ntcp::instance().send_server(nnodeid::nodeid(static_cast<int16_t>(tab->m_login), 1), pro, nguid::moreactor(), instance().id_guid());
	}

	bool actor_robot_manage::check_connect(i32_serverid aserverid, ENET_PROTOCOL aprotocol) const
	{
		net_works lnets;
		const tab_servers* ltab = ttab_servers::instance().tab(nnodeid::tid(aserverid));
		if (ltab == nullptr)
		{
			return false;
		}

		const ENET_PROTOCOL lprotocol = normalize_robot_protocol(aprotocol);
		if (lprotocol == ENET_WS)
		{
			if (ltab->m_type != NODE_TYPE::GATEWAY)
			{
				log_warn()->print("robot ws connect only supports gateway, server[{}] type[{}]", aserverid, static_cast<int32_t>(ltab->m_type));
				return false;
			}
			if (!ttab_servers::instance().get_nworks(ltab, ENET_WS, nnodeid::tcount(aserverid), lnets))
			{
				log_warn()->print("robot ws connect requested but gateway[{}] has no ws network", aserverid);
				return false;
			}
			return true;
		}

		return ttab_servers::instance().connect(aserverid, lnets);
	}

	void actor_robot_manage::connect(i32_serverid aserverid, ENET_PROTOCOL aprotocol, const std::function<void(i32_sessionid)>& afun) const
	{
		const ENET_PROTOCOL lprotocol = normalize_robot_protocol(aprotocol);
		if (!check_connect(aserverid, lprotocol))
		{
			return;
		}

		if (lprotocol == ENET_WS)
		{
			const tab_servers* llocaltab = ttab_servers::instance().const_tab();
			if (llocaltab == nullptr)
			{
				return;
			}
			nws::instance().init(0, llocaltab->m_threadnum, llocaltab->m_outernet, robot_gateway_use_tls(), robot_gateway_tls_options());
			nws::instance().connect(aserverid, afun, true, false);
			return;
		}

		ntcp::instance().connect(aserverid, afun, true, false);
	}

	bool actor_robot_manage::parse_command(std::string aparm)
	{
		auto ldata = std::make_shared<np_robot_pram>();
		ldata->m_cmd = std::move(aparm);
		i64_actorid lid = ngl::nguid::make(ACTOR_ROBOT_MANAGE, tab_self_area, nconfig.tid());
		handle_pram lparm = ngl::handle_pram::create<np_robot_pram, false, false>(lid, nguid::moreactor(), ldata);
		actor_manage::instance().push_task_id(lid, lparm);
		return true;
	}

	void actor_robot_manage::create_robot(const std::string& arobotname, ENET_PROTOCOL aprotocol) const
	{
		ngl::actor_robot_manage::login(arobotname, "123456", aprotocol);
	}

	void actor_robot_manage::create_robots(const std::string& arobotname, int abeg, int aend, ENET_PROTOCOL aprotocol) const
	{
		for (int i = abeg; i <= aend; ++i)
		{
			std::string lname(arobotname);
			lname += tools::lexical_cast<std::string>(i);
			ngl::actor_robot_manage::login(lname, "123456", aprotocol);
		}
	}

	void actor_robot_manage::foreach(const std::function<void(_robot&)>& afun)
	{
		for (std::pair<const std::string, _robot>& item : m_maprobot)
		{
			afun(item.second);
		}
	}

	_robot* actor_robot_manage::get_robot(const std::string& aacount)
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
		return arobot != nullptr;
	}


	bool actor_robot_manage::kcp_connect(i64_actorid arobotid, pbnet::ENUM_KCP akcpenum, int16_t aservertid, int16_t atcount, i64_actorid aseractorid)
	{
		int32_t lserverid = nnodeid::nodeid(aservertid, atcount);

		net_works lpstructserver;
		if (!ttab_servers::instance().get_nworks(lserverid, nconfig.area(), ENET_TCP, lpstructserver))
		{
			return false;
		}

		i16_port luport = nkcp::instance().kcp_port(aservertid, atcount, akcpenum);
		log_error()->print("kcp connect server[{}:{}] {}@{}", aservertid, atcount, lpstructserver.m_ip, luport);

		// Get uip
		ngl::asio_udp_endpoint lendpoint(ngl::basio_ipaddress::from_string(lpstructserver.m_ip), luport);
		i64_actorid robotid = nguid::make_type(arobotid, ACTOR_ROLE);
		_robot* lprobot = get_robot(robotid);
		if (lprobot == nullptr || lprobot->m_robot == nullptr)
		{
			return false;
		}
		
		lprobot->m_robot->kcp_setindex(lserverid, akcpenum, nkcp::instance().create_kcp());
		auto loptuport = lprobot->m_robot->kcp_index(lserverid, akcpenum);
		if (!loptuport.has_value())
		{
			return false;
		}
		auto lpukcp = nkcp::instance().kcp(*loptuport);
		if (lpukcp == nullptr)
		{
			return false;
		}

			lpukcp->sendu_waitrecv(lendpoint, "GetIp", sizeof("GetIp"), [this, &lprobot, lserverid, akcpenum, aseractorid](char* buff, [[maybe_unused]] int len)
			{
				log_error()->print("Local GetIp Finish : {}", buff);
				ukcp::m_localuip = buff;
				// Getkcp-session
				pbnet::PROBUFF_NET_KCPSESSION pro;
				pro.set_mserverid(lserverid);
				pro.set_muip(ukcp::m_localuip);
				auto luport2 = lprobot->m_robot->kcp_index(lserverid, akcpenum);
				if (!luport2.has_value())
				{
					return;
				}
				pro.set_muport(*luport2);
				pro.set_mconv(ukcp::m_conv);
				pro.set_mactoridclient(lprobot->m_robot->id_guid());
				pro.set_mactoridserver(aseractorid);
				pro.set_m_kcpnum(akcpenum);
				nnet::instance().send(lprobot->m_session, pro, nguid::moreactor(), lprobot->m_robot->id_guid());
			}
		);
		return true;
	}


	bool actor_robot_manage::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl
