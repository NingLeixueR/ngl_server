#pragma once

#include "init_server.h"

#define DEF_COUNT (2000)
void init_DB_ACCOUNT(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_account ltemp;
		ltemp.set_m_account(std::string(aname) + boost::lexical_cast<std::string>(i % DEF_COUNT));
		ltemp.set_m_id(ngl::actor_guid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i));
		ltemp.set_m_passworld("123456");
		ltemp.set_m_roleid(ltemp.m_id());
		ltemp.set_m_area(ngl::tab_self_area);
		ngl::actor_dbtab<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_ACCOUNT, pbdb::db_account>::save(0, ltemp);

		ngl::actor_dbtab<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_ACCOUNT, pbdb::db_account>::load(0, ngl::actor_guid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, 1));
		pbdb::db_account lDB_ACCOUNT;
		ngl::dbdata<pbdb::db_account>::get(ngl::actor_guid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, 1), lDB_ACCOUNT);
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

		ngl::i64_actorid lid = ngl::actor_guid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i);
		ltemp.set_m_id(lid);
		pbdb::db_brief* lrolebase = ltemp.mutable_m_base();
		lrolebase->set_m_id(lid);
		lrolebase->set_m_name(std::string(aname) + boost::lexical_cast<std::string>(i % DEF_COUNT));
		lrolebase->set_m_lv(i);
		lrolebase->set_m_moneygold(i + 1000);
		lrolebase->set_m_moneysilver(i + 2000);

		ngl::actor_dbtab<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_ROLE, pbdb::db_role>::save(0, ltemp);
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
		ltemp.set_m_id(ngl::actor_guid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i));
		ltemp.set_m_maxid(1);

		ngl::actor_dbtab<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_BAG, pbdb::db_bag>::save(0, ltemp);
	}
}

void init_DB_BAG()
{
	init_DB_BAG("libo", (0 * DEF_COUNT) + 1);
	init_DB_BAG("wac", (1 * DEF_COUNT) + 1);
}

//DB_MAIL
void init_DB_MAIL(int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_mail ltemp;
		ltemp.set_m_id(ngl::actor_guid::make(ngl::ACTOR_ROLE, ngl::tab_self_area, i));
		ngl::actor_dbtab<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_MAIL, pbdb::db_mail>::save(0, ltemp);
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


		if (ngl::conversion::to_utf8(lvec[i], lvec[i]) == false)
			continue;

		ltemp.set_m_notice(lvec[i]);
		ltemp.set_m_starttime(time(NULL));
		ltemp.set_m_finishtime(time(NULL) + 36000);
		ngl::actor_dbtab<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_NOTICE, pbdb::db_notice>::save(0, ltemp);
	}
}


bool start_db(int argc, char** argv)
{
	LogLocalError("[%] start", "DB");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::tab_servers* tab = ngl::ttab_servers::tab();	
	if (!ngl::db_pool::init(tab->m_actorthreadnum, nconfig::m_db.m_dbarg))
		return false;

	ngl::actor_client::getInstance();

	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);

	using actor_db_account = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_ACCOUNT, pbdb::db_account>;
	actor_db_account::getInstance();

	using actor_db_role = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_ROLE, pbdb::db_role>;
	actor_db_role::getInstance();

	using actor_db_bag = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_BAG, pbdb::db_bag>;
	actor_db_bag::getInstance();

	using actor_db_keyvalue = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_KEYVALUE, pbdb::db_keyvalue>;
	actor_db_keyvalue::getInstance();

	using actor_db_mail = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_MAIL, pbdb::db_mail>;
	actor_db_mail::getInstance();

	using actor_db_guild = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_GUILD, pbdb::db_guild>;
	actor_db_guild::getInstance();

	using actor_db_notice = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_NOTICE, pbdb::db_notice>;
	actor_db_notice::getInstance();

	using actor_db_activity = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_ACTIVITY, pbdb::db_activity>;
	actor_db_activity::getInstance();

	using actor_db_brief = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_BRIEF, pbdb::db_brief>;
	actor_db_brief::getInstance();

	using actor_db_ranklist = ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, ngl::ENUM_DB_RANKLIST, pbdb::db_ranklist>;
	actor_db_ranklist::getInstance();

	ngl::actor_client::getInstance().actor_server_register();

	// ----------------test start-------------------- //
	if (argc >= 5 && ngl::tools::is_mem(argv[4], "init"))
	{
		init_DB_ACCOUNT();
		init_DB_ROLE();
		init_DB_BAG();
		init_DB_NOTICE();
	}

	return true;
}

bool start_world()
{
	LogLocalError("[%] start", "WORLD");
	//config.m_nodetype = ngl::configxml_node::ACTORSERVER;

	if (!init_server(nconfig::m_nodeid))
		return false;

	
	ngl::actor_client::getInstance();

	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);
	ngl::actor_notice::getInstance();
	ngl::actor_gm::getInstance();
	ngl::actor_mail::getInstance();
	ngl::actor_chat::getInstance();
	ngl::actor_manage_activity::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_login()
{
	//ngl::logdef::m_loglevel = ngl::DO_NOT_PRINT;
	LogLocalError("[%] start", "LOGIN");
	//config.m_nodetype = ngl::configxml_node::LOGIN;

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();
	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);
	ngl::actor_login::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_gateway()
{
	//ngl::logdef::m_loglevel = ngl::DO_NOT_PRINT;
	LogLocalError("[%] start", "GATEWAY");
	//config.m_nodetype = ngl::configxml_node::GATEWAY;

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);
	ngl::actor_gateway::getInstance();
	ngl::actor_gateway_game2client::getInstance();
	ngl::actor_gateway_client2game::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_log()
{
	//ngl::logdef::m_loglevel = ngl::DO_NOT_PRINT;
	LogLocalError("[%] start", "LOG");
	//config.m_nodetype = ngl::configxml_node::LOG;

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);

	ltype = ngl::ELOG_BI;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);

	ltype = ngl::ELOG_DEFAULT;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_actor()
{
	LogLocalError("[%] start", "ACTORSERVER");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_server::getInstance();
	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);
	return true;
}

bool start_game()
{
	LogLocalError("[%] start", "GAME");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();
	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);
	ngl::actor_manage_role::getInstance();
	ngl::actor_switchprocess::getInstance();
	ngl::actor_brief::getInstance();
	//ngl::actor_reloadcsv::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_cross()
{
	LogLocalError("[%] start", "CROSS");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();
	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);
	ngl::actor_chat::getInstance();
	//ngl::actor_reloadcsv::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_reloadcsv()
{
	LogLocalError("[%] start", "RELOADCSV");

	if (!init_server(nconfig::m_nodeid))
		return false;

	ngl::actor_client::getInstance();

	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);
	ngl::actor_reloadcsv_distribute::getInstance();

	ngl::actor_client::getInstance().actor_server_register();
	return true;
}

bool start_robot(int argc, char** argv)
{
	//ngl::logdef::m_loglevel = ngl::DO_NOT_PRINT;
	LogLocalError("[%] start", "ROBOT");
	//config.m_nodetype = ngl::configxml_node::ROBOT;

	if (!init_server(nconfig::m_nodeid))
		return 0;

	ngl::actor_client::getInstance();
	ngl::ELOG_TYPE ltype = ngl::ELOG_LOCAL;
	ngl::actor_base::create(ngl::ACTOR_LOG, 0, &ltype);
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
			ngl::splite::division(lbuff, " ", lvec);
			ngl::robot_cmd::parse_command(lvec);
		}
	}
	else
	{
		for (int i = 0; i < 15; ++i)
		{
			boost::this_thread::sleep(boost::posix_time::seconds(1));
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
		ngl::splite::division(lcmd.c_str(), " ", lvec);
		ngl::robot_cmd::parse_command(lvec);
		std::cout << "---------------[" << lcmd << "]---------------" << std::endl;
		for (int i = 0; i < 1; ++i)
		{
			boost::this_thread::sleep(boost::posix_time::seconds(1));
			std::cout << "---------------[" << i << "]---------------" << std::endl;
		}
		int lnum = 10000;
		//boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
		//while (lnum-- > 0)

		bool ltest = false;
		std::vector<int> lms;
		std::vector<std::vector<std::string>> lcmdvec;
		boost::thread lthread([&ltest, &lms, &lcmdvec]()
			{
				std::string lcmd;
				std::vector<std::string> lvec;
				while (true)
				{
					lcmd.clear();
					char lbuff[1024] = { 0x0 };
					std::cin.getline(lbuff, 1024);
					lcmd = lbuff;
					ngl::splite::division(lcmd.c_str(), " ", lvec);
					if (lvec[0] == "test" || lvec[0] == "TEST")
					{
						lms.clear();
						lms.push_back(boost::lexical_cast<int>(lvec[1]));
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
						lms.push_back(boost::lexical_cast<int>(lvec[1]));
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
					ngl::splite::division(lcmd.c_str(), " ", lvec);
					ngl::robot_cmd::parse_command(lvec);
				}
			});

		for (int i = 0;; ++i)
		{
			if (!ltest)
			{
				boost::this_thread::sleep(boost::posix_time::seconds(1));
				continue;
			}			
			for (int j = 0; j < lcmdvec.size() && j < lms.size(); ++j)
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(lms[j]));
				std::vector<std::string> lcmdvec2 = lcmdvec[j];
				lcmdvec2.push_back(boost::lexical_cast<std::string>(i));
				ngl::robot_cmd::parse_command(lcmdvec2);
			}
		}

	}

	return true;
}