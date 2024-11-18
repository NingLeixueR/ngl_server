#pragma once

#include "init_server.h"

#define DEF_COUNT (2000)

void init_DB_ACCOUNT(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_account ltemp;
		ltemp.set_m_account(std::string(aname) + ngl::tools::lexical_cast<std::string>(i % DEF_COUNT));
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltemp.set_m_passworld("123456");
		ltemp.set_m_roleid(ltemp.m_id());
		ltemp.set_m_area(tab_self_area);
		ngl::actor_dbtab<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>::save(0, ltemp);

		ngl::actor_dbtab<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>::load(0, ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, 1));
		pbdb::db_account lDB_ACCOUNT;
		ngl::db_data<pbdb::db_account>::get(
			ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, 1), 
			lDB_ACCOUNT
		);
	}
}

void init_DB_ACCOUNT()
{
	init_DB_ACCOUNT("libo", (0 * DEF_COUNT) + 1);
	init_DB_ACCOUNT("wac", (1 * DEF_COUNT) + 1);
}

//DB_ROLE
void init_DB_ROLE(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_role ltemp;

		ngl::i64_actorid lid = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i);
		ltemp.set_m_id(lid);
		pbdb::db_brief* lrolebase = ltemp.mutable_m_base();
		lrolebase->set_m_id(lid);
		lrolebase->set_m_name(std::string(aname) + ngl::tools::lexical_cast<std::string>(i % DEF_COUNT));
		lrolebase->set_m_lv(i);
		lrolebase->set_m_moneygold(i + 1000);
		lrolebase->set_m_moneysilver(i + 2000);
		lrolebase->set_m_createutc(ngl::localtime::gettime());

		ngl::actor_dbtab<pbdb::ENUM_DB_ROLE, pbdb::db_role>::save(0, ltemp);
	}
}

void init_DB_ROLE()
{
	std::cout << "#########init_DB_ROLE()#############" << std::endl;
	init_DB_ROLE("libo", (0 * DEF_COUNT) + 1);
	init_DB_ROLE("wac", (1 * DEF_COUNT) + 1);
}

//DB_BAG
void init_DB_BAG(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_bag ltemp;
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltemp.set_m_maxid(1);

		ngl::actor_dbtab<pbdb::ENUM_DB_BAG, pbdb::db_bag>::save(0, ltemp);
	}
}

void init_DB_BAG()
{
	init_DB_BAG("libo", (0 * DEF_COUNT) + 1);
	init_DB_BAG("wac", (1 * DEF_COUNT) + 1);
}

void init_DB_TASK(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_task ltemp;
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		auto lrundatas = ltemp.mutable_m_rundatas();
		std::pair<int32_t, pbdb::db_task::data> lpair;
		lpair.first = 1;
		lpair.second.set_m_taskid(1);
		lpair.second.set_m_receiveutc(0);
		lpair.second.set_m_finshutc(0);
		auto lschedules = lpair.second.mutable_m_schedules();
		auto lschedulesnode = lschedules->Add();
		lschedulesnode->set_m_value(1);
		lschedulesnode->set_m_sumint(10);
		(*lrundatas)[lpair.first] = lpair.second;

		ngl::actor_dbtab<pbdb::ENUM_DB_TASK, pbdb::db_task>::save(0, ltemp);
	}
}

void init_DB_TASK()
{
	init_DB_TASK("libo", (0 * DEF_COUNT) + 1);
	init_DB_TASK("wac", (1 * DEF_COUNT) + 1);
}

//DB_MAIL
void init_DB_MAIL(int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_mail ltemp;
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ngl::actor_dbtab<pbdb::ENUM_DB_MAIL, pbdb::db_mail>::save(0, ltemp);
	}
}

void init_DB_MAIL()
{
	init_DB_MAIL((0 * DEF_COUNT) + 1);
	init_DB_MAIL((1 * DEF_COUNT) + 1);
}

void init_DB_ROLEKEYVALUE(int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_rolekeyvalue ltemp;
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		(*ltemp.mutable_m_data())["test1"] = "1";
		(*ltemp.mutable_m_data())["test2"] = "2";
		(*ltemp.mutable_m_data())["test3"] = "3";

		ngl::actor_dbtab<pbdb::ENUM_DB_ROLEKEYVALUE, pbdb::db_rolekeyvalue>::save(0, ltemp);
	}
}

void init_DB_ROLEKEYVALUE()
{
	init_DB_ROLEKEYVALUE((0 * DEF_COUNT) + 1);
	init_DB_ROLEKEYVALUE((1 * DEF_COUNT) + 1);
}

void init_DB_NOTICE()
{
	std::vector<std::string> lvec =
	{
		"һ�仰", // 1
		"һ����", // 2
		"һ����", // 3
		"������������������", // 4
		"����ʹ��", // 5
		"ţ�ƿ���˹", // 6
		"ս�������", // 7
		"�Ժ�����˯", // 8
		"��ֲΪ��", // 9
		"����32��", // 10
	};
	for (int i = 1; i < 10; ++i)
	{
		pbdb::db_notice ltemp;
		ltemp.set_m_id(i);


		if (ngl::tools::to_utf8(lvec[i], lvec[i]) == false)
			continue;

		ltemp.set_m_notice(lvec[i]);
		ltemp.set_m_starttime(time(NULL));
		ltemp.set_m_finishtime(time(NULL) + 36000);
		ngl::actor_dbtab<pbdb::ENUM_DB_NOTICE, pbdb::db_notice>::save(0, ltemp);
	}
}

void init_DB_KEYVAL()
{
	pbdb::db_keyvalue ltemp;
	ltemp.set_m_id(pbdb::db_keyvalue_ekv_none);
	int lnow = ngl::localtime::gettime();
	std::string ltempstr = std::format("{}*{}", lnow, ngl::localtime::time2str(lnow, "%y/%m/%d %H:%M:%S"));
	ltemp.set_m_value(ltempstr);
	ngl::actor_dbtab<pbdb::ENUM_DB_KEYVALUE, pbdb::db_keyvalue>::save(0, ltemp);
}

void init_DB_FAMILY()
{
	for (int i = 1; i < 100; ++i)
	{
		pbdb::db_family ltemp;

		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_FAMILY, tab_self_area, i));
		ltemp.set_m_createutc(ngl::localtime::gettime());
		ltemp.set_m_exp(100);
		ltemp.set_m_lv(1);

		ltemp.set_m_name(std::format("FLIBO{}", i));
		ltemp.set_m_leader(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		*ltemp.mutable_m_member()->Add() = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i);
		ngl::actor_dbtab<pbdb::ENUM_DB_FAMILY, pbdb::db_family>::save(0, ltemp);

		pbdb::db_familyer ltempfamilyer;
		ltempfamilyer.set_m_joinutc(ngl::localtime::gettime());
		ltempfamilyer.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltempfamilyer.set_m_position(pbdb::db_familyer_eposition_leader);
		ltempfamilyer.set_m_lastsignutc(ngl::localtime::gettime());
		ngl::actor_dbtab<pbdb::ENUM_DB_FAMILYER, pbdb::db_familyer>::save(0, ltempfamilyer);
	}
}

void init_DB_RANKLIST()
{
	for (int i = 1; i < 100; ++i)
	{
		pbdb::db_ranklist ltemp;
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		pbdb::rankitem lrankitem;
		lrankitem.set_m_time(ngl::localtime::gettime());
		lrankitem.set_m_value(i);
		(*ltemp.mutable_m_items())[(int)pbdb::eranklist::lv] = lrankitem;
		ngl::actor_dbtab<pbdb::ENUM_DB_RANKLIST, pbdb::db_ranklist>::save(0, ltemp);
	}
}

void init_DB_FRIENDS()
{
	std::map<ngl::i64_actorid, pbdb::db_friends> lmap;
	for (int i = 0; i < 10; ++i)
	{
		int lbeg = i * 10;
		int lend = lbeg + 10;
		for (int j1 = lbeg; j1 < lend; ++j1)
		{
			ngl::i64_actorid lactor1 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, j1);
			pbdb::db_friends& lfriends = lmap[lactor1];
			lfriends.set_m_id(lactor1);
			for (int j2 = lbeg; j2 < lend; ++j2)
			{
				if (j1 != j2)
				{
					ngl::i64_actorid lactor2 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, j2);
					ngl::i64_actorid lactor3 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, j2+10);
					lfriends.add_m_friends(lactor2);
					lfriends.add_m_applyfriends(lactor3);
				}
			}
		}
	}

	std::ranges::for_each(lmap, [](const auto& apair)
		{
			ngl::actor_dbtab<pbdb::ENUM_DB_FRIENDS, pbdb::db_friends>::save(0, apair.second);
		});
}


bool start_db(int argc, char** argv)
{
	ngl::log_error()->print("[{}] start", "DB");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);

	ngl::init_nactor_type(false);
	ngl::actor_gmclient::getInstance();
	
	ngl::actor_client::getInstance().actor_server_register();

	// ----------------test start-------------------- //
	if (argc >= 5)
	{
		std::string largv4(argv[4]);
		if (largv4 == "init")
		{
			init_DB_BAG();
			init_DB_TASK();
			init_DB_ROLE();
			init_DB_NOTICE();
			init_DB_FAMILY();
			init_DB_KEYVAL();
			init_DB_ACCOUNT();
			init_DB_ROLEKEYVALUE();
			init_DB_RANKLIST();
			init_DB_FRIENDS();
		}		
	}

	return true;
}

bool start_world()
{
	ngl::log_error()->print("[{}] start", "WORLD");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);

	ngl::actor_events<ngl::ENUM_EVENTS_LOGIC, ngl::eevents_logic>::getInstance();
	ngl::actor_gm::getInstance();
	ngl::actor_mail::getInstance();
	ngl::actor_chat::getInstance();
	ngl::actor_brief::getInstance();
	ngl::actor_notice::getInstance();
	ngl::actor_keyvalue::getInstance();
	ngl::actor_manage_activity::getInstance();
	ngl::actor_family::getInstance();
	ngl::actor_ranklist::getInstance();
	ngl::actor_friends::getInstance();
	ngl::actor_gmclient::getInstance();


	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_login()
{
	ngl::log_error()->print("[{}] start", "LOGIN");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);

	ngl::actor_login::getInstance();
	ngl::actor_gmclient::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_gateway()
{
	ngl::log_error()->print("[{}] start", "GATEWAY");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);

	ngl::actor_gateway::getInstance();
	ngl::actor_gatewayg2c::getInstance();
	ngl::actor_gatewayc2g::getInstance();
	ngl::actor_gmclient::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_log()
{
	ngl::log_error()->print("[{}] start", "LOG");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);
	ngl::actor_gmclient::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_actor()
{
	ngl::log_error()->print("[{}] start", "ACTORSERVER");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_server::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);
	ngl::actor_gmclient::getInstance();

	return true;
}

bool start_game()
{
	ngl::log_error()->print("[{}] start", "GAME");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);

	ngl::actor_manage_role::getInstance();
	ngl::actor_create::getInstance();
	ngl::actor_kcp::getInstance();
	ngl::actor_calendar::getInstance();
	ngl::actor_gmclient::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_cross()
{
	ngl::log_error()->print("[{}] start", "CROSS");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);

	ngl::actor_chat::getInstance();
	ngl::actor_gmclient::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_pushserverconfig()
{
	// �������������ϴ�lbgmsys
	ngl::xmlinfo* lpublicxml = ngl::xmlnode::get_publicconfig();
	std::string lgmurl;
	if (!lpublicxml->find("gmurl", lgmurl))
		return false;
	std::string lpushserver;
	if (!lpublicxml->find("push_server_config", lpushserver))
		return false;
	lpushserver = lgmurl + "/" + lpushserver;

	ngl::ttab_servers::foreach_server([&lpushserver](const ngl::tab_servers* aserver)
		{
			auto lhttp = ngl::manage_curl::make_http();
			ngl::manage_curl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
			ngl::manage_curl::set_type(lhttp, ngl::ENUM_TYPE_GET);
			ngl::manage_curl::set_url(lhttp, lpushserver);

			std::stringstream lstream;
			lstream
				<< "id=" << aserver->m_id
				<< "&name=" << aserver->m_name
				<< "&area=" << aserver->m_area
				<< "&type=" << (int32_t)aserver->m_type;

			//  ENET_TCP = 0,
			//	ENET_WS = 1,
			//	ENET_KCP = 2,
			ngl::json_write lwrite;
			std::array<std::string, ngl::ENET_COUNT> lparm = { "tcp","ws","kcp" };
			for (const ngl::net_works& item : aserver->m_net)
			{
				ngl::json_write lwritetemp;
				lwritetemp.write(
					"ip", item.m_ip, 
					"nip", item.m_nip,
					"port", item.m_port
				);
				lwrite.write(lparm[item.m_type].c_str(), lwritetemp);
			}

			lwrite.set_nonformatstr(true);
			std::string lnet;
			lwrite.get(lnet);
			lstream << "&net=" << lnet;

			std::cout << lnet << std::endl;
			
			std::string lstr = lstream.str();
			ngl::manage_curl::set_param(lhttp, lstr);

			ngl::manage_curl::set_callback(lhttp, [lstr](int, ngl::_http& ahttp)
				{
					ngl::log_error()->print("[{}]->[{}]", lstr, ahttp.m_recvdata);
				});
			ngl::manage_curl::send(lhttp);
		});
	return true;
}

bool start_csvserver()
{
	ngl::log_error()->print("[{}] start", "RELOADCSV");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);

	ngl::actor_csvserver::getInstance();
	ngl::actor_gmclient::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_robot(int argc, char** argv)
{
	ngl::log_error()->print("[{}] start", "ROBOT");

	if (!init_server(nconfig::m_nodeid))
		return 0;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);

	ngl::actor_gmclient::getInstance();

	ngl::actor_manage_robot::getInstance();

	ngl::actor_manage_robot::getInstance().connect(ngl::ttab_servers::tab()->m_login, [](int asession)
		{
			std::cout << "����Login�������ɹ�" << std::endl;
		});
	if (argc < 4)
	{
		while (1)
		{
			char lbuff[4096] = { 0 };
			std::cin.getline(lbuff, 4096);
			std::vector<std::string> lvec;
			if (ngl::tools::splite(lbuff, " ", lvec) == false)
				continue;
			ngl::actor_manage_robot::parse_command(lvec);
		}
	}
	else
	{
		for (int i = 0; i < 5; ++i)
		{
			ngl::sleep::seconds(1);
			std::cout << "---------------[" << i << "]---------------" << std::endl;
		}
		std::string lcmd;
		if (argc < 6)
		{
			lcmd = "login ";
			lcmd += argv[4];
		}
		else
		{
			lcmd = "logins ";
			lcmd += argv[4];
			lcmd += " ";
			lcmd += argv[5];
			lcmd += " ";
			lcmd += argv[6];
		}
		std::vector<std::string> lvec;
		if (ngl::tools::splite(lcmd.c_str(), " ", lvec) == false)
			return false;
		ngl::actor_manage_robot::parse_command(lvec);
		int lnum = 10000;
		//boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
		//while (lnum-- > 0)

		bool ltest = false;
		std::vector<int> lms;
		std::vector<std::vector<std::string>> lcmdvec;
		std::thread lthread([&ltest, &lms, &lcmdvec]()
			{
				std::string lcmd;
				std::vector<std::string> lvec;
				while (true)
				{
					lcmd.clear();
					char lbuff[1024] = { 0x0 };
					std::cin.getline(lbuff, 1024);
					lcmd = lbuff;
					lvec.clear();
					if (ngl::tools::splite(lcmd.c_str(), " ", lvec) == false)
						continue;
					if (lvec[0] == "test" || lvec[0] == "TEST")
					{
						lms.clear();
						lms.push_back(ngl::tools::lexical_cast<int>(lvec[1].c_str()));
						lcmdvec.clear();
						lcmdvec.push_back(std::vector<std::string>());
						for (int i = 2; i < lvec.size(); ++i)
						{
							lcmdvec.rbegin()->push_back(lvec[i]);
						}
						ltest = true;
						continue;
					}
					else if (lvec[0] == "notest" || lvec[0] == "NOTEST")
					{
						lms.clear();
						lcmdvec.clear();
						ltest = false;
						continue;
					}
					else if (lvec[0] == "tests" || lvec[0] == "TESTS")
					{
						lms.push_back(ngl::tools::lexical_cast<int>(lvec[1].c_str()));
						lcmdvec.push_back(std::vector<std::string>());
						for (int i = 2; i < lvec.size(); ++i)
						{
							lcmdvec.rbegin()->push_back(lvec[i]);
						}
						continue;
					}
					else if (lvec[0] == "start" || lvec[0] == "START")
					{
						ltest = true;
						continue;
					}
					ngl::actor_manage_robot::parse_command(lvec);
				}
			});

		for (int i = 0;; ++i)
		{
			if (!ltest)
			{
				ngl::sleep::seconds(1);
				continue;
			}			
			for (int j = 0; j < lcmdvec.size() && j < lms.size(); ++j)
			{
				ngl::sleep::milliseconds(lms[j]);
				std::vector<std::string> lcmdvec2 = lcmdvec[j];
				lcmdvec2.push_back(ngl::tools::lexical_cast<std::string>(i));
				ngl::actor_manage_robot::parse_command(lcmdvec2);
			}
		}
	}
	return true;
}