#pragma once

#include "init_server.h"

#define DEF_COUNT (2000)
void init_DB_ACCOUNT(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_account ltemp;
		ltemp.set_m_account(std::string(aname) + ngl::tools::lexical_cast<std::string>(i % DEF_COUNT));
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i));
		ltemp.set_m_passworld("123456");
		ltemp.set_m_roleid(ltemp.m_id());
		ltemp.set_m_area(ngl::tab_self_area);
		ngl::actor_dbtab<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>::save(0, ltemp);

		ngl::actor_dbtab<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>::load(0, ngl::nguid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, 1));
		pbdb::db_account lDB_ACCOUNT;
		ngl::db_data<pbdb::db_account>::get(
			ngl::nguid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, 1), 
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

		ngl::i64_actorid lid = ngl::nguid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i);
		ltemp.set_m_id(lid);
		pbdb::db_brief* lrolebase = ltemp.mutable_m_base();
		lrolebase->set_m_id(lid);
		lrolebase->set_m_name(std::string(aname) + ngl::tools::lexical_cast<std::string>(i % DEF_COUNT));
		lrolebase->set_m_lv(i);
		lrolebase->set_m_moneygold(i + 1000);
		lrolebase->set_m_moneysilver(i + 2000);

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
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i));
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
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i));
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
		ltemp.set_m_id(ngl::nguid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i));
		ngl::actor_dbtab<pbdb::ENUM_DB_MAIL, pbdb::db_mail>::save(0, ltemp);
	}
}

void init_DB_MAIL()
{
	init_DB_MAIL((0 * DEF_COUNT) + 1);
	init_DB_MAIL((1 * DEF_COUNT) + 1);
}

void init_DB_NOTICE()
{
	std::vector<std::string> lvec =
	{
		"一句话", // 1
		"一辈子", // 2
		"一生情", // 3
		"哈哈哈哈哈哈哈哈哈", // 4
		"护花使者", // 5
		"牛逼克拉斯", // 6
		"战天城问问", // 7
		"吃喝拉撒睡", // 8
		"妖植为何", // 9
		"银月32城", // 10
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


bool start_db(int argc, char** argv)
{
	ngl::log_error()->print("[{}] start", "DB");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::tab_servers* tab = ngl::ttab_servers::tab();	
	if (!ngl::db_pool::init(tab->m_actorthreadnum, nconfig::m_db.m_dbarg))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);

	ngl::init_nactor_type(false);
	
	ngl::actor_client::getInstance().actor_server_register();

	// ----------------test start-------------------- //
	if (argc >= 5 && ngl::tools::is_equal(argv[4], "init"))
	{
		init_DB_ACCOUNT();
		init_DB_ROLE();
		init_DB_BAG();
		init_DB_NOTICE();
		init_DB_TASK();
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
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);

	ngl::actor_notice::getInstance();
	ngl::actor_gm::getInstance();
	ngl::actor_mail::getInstance();
	ngl::actor_chat::getInstance();
	ngl::actor_brief::getInstance();
	ngl::actor_manage_activity::getInstance();

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
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);

	ngl::actor_login::getInstance();

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
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);

	ngl::actor_gateway::getInstance();
	ngl::actor_gatewayg2c::getInstance();
	ngl::actor_gatewayc2g::getInstance();

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
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);

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
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);


	return true;
}

bool start_game()
{
	ngl::log_error()->print("[{}] start", "GAME");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::nlogactor lnlogactor(ngl::ACTOR_NONE, ngl::ELOG_LOCAL);
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);

	ngl::actor_manage_role::getInstance();
	ngl::actor_create::getInstance();
	ngl::actor_kcp::getInstance();
	ngl::actor_calendar::getInstance();
	//ngl::actor_reloadcsv::getInstance();

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
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);

	ngl::actor_chat::getInstance();
	//ngl::actor_reloadcsv::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_pushserverconfig()
{
	// 将服务器配置上传lbgmsys
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
				<< "&area=" << aserver->m_area;

			//  ENET_TCP = 0,
			//	ENET_WS = 1,
			//	ENET_KCP = 2,
			ngl::json_write lwrite;
			std::array<std::string, ngl::ENET_COUNT> lparm = { "tcp","ws","kcp" };
			for (const ngl::net_works& item : aserver->m_net)
			{
				ngl::json_write lwritetemp;
				lwritetemp.write("type", (int8_t)item.m_type);
				lwritetemp.write("ip", item.m_ip);
				lwritetemp.write("nip", item.m_nip);
				lwritetemp.write("port", item.m_port);
				lwrite.write(lparm[item.m_type].c_str(), lwritetemp);
			}
			std::string lnet;
			lwrite.get(lnet);
			lstream << "&net=" << lnet;
			
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
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);

	ngl::actor_csvserver::getInstance();

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
	ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);


	ngl::actor_manage_robot::getInstance();

	ngl::actor_manage_robot::getInstance().connect(ngl::ttab_servers::tab()->m_login, [](int asession)
		{
			std::cout << "连接Login服务器成功" << std::endl;
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
					if (ngl::tools::splite(lcmd.c_str(), " ", lvec) == false)
						continue;
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