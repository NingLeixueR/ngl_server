#include "server_main.h"
#include "init_server.h"
#include "xml_serialize.h"

#define DEF_COUNT (2000)

void init_DB_ACCOUNT(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_account ltemp;
		ltemp.set_maccount(std::string(aname) + ngl::tools::lexical_cast<std::string>(i % DEF_COUNT));
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltemp.set_mpassworld("123456");
		ltemp.set_mroleid(ltemp.mid());
		ltemp.set_marea(tab_self_area);

		ngl::ndbtab<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>::save(0, ltemp);
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
		ltemp.set_mid(lid);
		pbdb::db_brief lrolebase;
		lrolebase.set_mid(ngl::nguid::make(ngl::ACTOR_BRIEF, tab_self_area, i));
		lrolebase.mutable_mbase()->set_mname(std::string(aname) + ngl::tools::lexical_cast<std::string>(i % DEF_COUNT));
		lrolebase.mutable_mbase()->set_mlv(i);
		lrolebase.mutable_mbase()->set_mmoneygold(0);
		lrolebase.mutable_mbase()->set_mmoneysilver(0);
		lrolebase.mutable_mbase()->set_mcreateutc((int32_t)ngl::localtime::gettime());

		ngl::ndbtab<pbdb::ENUM_DB_BRIEF, pbdb::db_brief>::save(0, lrolebase);
		ngl::ndbtab<pbdb::ENUM_DB_ROLE, pbdb::db_role>::save(0, ltemp);
	}
}

void init_DB_ROLE()
{
	std::cout << "#########init_DB_ROLE()#############" << std::endl;
	std::string lstr = std::format("{}_zone{}_", "libo", nconfig.area());
	init_DB_ROLE(lstr.c_str(), (0 * DEF_COUNT) + 1);
	lstr = std::format("{}_zone{}_", "wac", nconfig.area());
	init_DB_ROLE(lstr.c_str(), (1 * DEF_COUNT) + 1);
}

//DB_BAG
void init_DB_BAG(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_bag ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltemp.set_mmaxid(1);

		ngl::ndbtab<pbdb::ENUM_DB_BAG, pbdb::db_bag>::save(0, ltemp);
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
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		auto lrundatas = ltemp.mutable_mrundatas();
		std::pair<int32_t, pbdb::db_task::data> lpair;
		lpair.first = 1;
		lpair.second.set_mtaskid(1);
		lpair.second.set_mreceiveutc(0);
		lpair.second.set_mfinshutc(0);
		auto lschedules = lpair.second.mutable_mschedules();
		auto lschedulesnode = lschedules->Add();
		lschedulesnode->set_mvalue(1);
		lschedulesnode->set_msumint(10);
		(*lrundatas)[lpair.first] = lpair.second;

		ngl::ndbtab<pbdb::ENUM_DB_TASK, pbdb::db_task>::save(0, ltemp);
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
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));

		ngl::ndbtab<pbdb::ENUM_DB_MAIL, pbdb::db_mail>::save(0, ltemp);
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
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		(*ltemp.mutable_mdata())["test1"] = "1";
		(*ltemp.mutable_mdata())["test2"] = "2";
		(*ltemp.mutable_mdata())["test3"] = "3";

		ngl::ndbtab<pbdb::ENUM_DB_ROLEKEYVALUE, pbdb::db_rolekeyvalue>::save(0, ltemp);
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
		ltemp.set_mid((ngl::ttab_servers::instance().const_tab()->m_area * 100) + i);
		if (ngl::tools::to_utf8(lvec[i], lvec[i]) == false)
			continue;
		ltemp.set_mnotice(lvec[i]);
		ltemp.set_mstarttime((int32_t)time(nullptr));
		ltemp.set_mfinishtime((int32_t)time(nullptr) + 36000);

		ngl::ndbtab<pbdb::ENUM_DB_NOTICE, pbdb::db_notice>::save(0, ltemp);
	}
}

void init_DB_KEYVAL()
{
	pbdb::db_keyvalue ltemp;
	ltemp.set_mid(pbdb::db_keyvalue_ekv_none);
	int32_t lnow = (int32_t)ngl::localtime::gettime();
	std::string ltempstr = std::format("{}*{}", lnow, ngl::localtime::time2str(lnow, "%y/%m/%d %H:%M:%S"));
	ltemp.set_mvalue(ltempstr);

	ngl::ndbtab<pbdb::ENUM_DB_KEYVALUE, pbdb::db_keyvalue>::save(0, ltemp);
}

void init_DB_FAMILY()
{
	for (int i = 1; i < 100; ++i)
	{
		pbdb::db_family ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_FAMILY, tab_self_area, i));
		ltemp.set_mcreateutc((int32_t)ngl::localtime::gettime());
		ltemp.set_mexp(100);
		ltemp.set_mlv(1);
		ltemp.set_mname(std::format("FLIBO{}", i));
		ltemp.set_mleader(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		*ltemp.mutable_mmember()->Add() = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i);

		ngl::ndbtab<pbdb::ENUM_DB_FAMILY, pbdb::db_family>::save(0, ltemp);

		pbdb::db_familyer ltempfamilyer;
		ltempfamilyer.set_mjoinutc((int32_t)ngl::localtime::gettime());
		ltempfamilyer.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltempfamilyer.set_mposition(pbdb::db_familyer_eposition_leader);
		ltempfamilyer.set_mlastsignutc((int32_t)ngl::localtime::gettime());

		ngl::ndbtab<pbdb::ENUM_DB_FAMILYER, pbdb::db_familyer>::save(0, ltempfamilyer);
	}
}

void init_DB_RANKLIST()
{
	/*for (int i = 1; i < 100; ++i)
	{
		pbdb::db_ranklist ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		pbdb::rankitem lrankitem;
		lrankitem.set_mtime((int32_t)ngl::localtime::gettime());
		lrankitem.set_mvalue(i);
		(*ltemp.mutable_mitems())[(int)pbdb::eranklist::lv] = lrankitem;

		ngl::ndbtab<pbdb::ENUM_DB_RANKLIST, pbdb::db_ranklist>::save(0, ltemp);
	}*/
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
			lfriends.set_mid(lactor1);
			for (int j2 = lbeg; j2 < lend; ++j2)
			{
				if (j1 != j2)
				{
					ngl::i64_actorid lactor2 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, j2);
					ngl::i64_actorid lactor3 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, j2 + 10);
					lfriends.add_mfriends(lactor2);
					lfriends.add_mapplyfriends(lactor3);
				}
			}
		}
	}

	for (const auto& apair : lmap)
	{
		ngl::ndbtab<pbdb::ENUM_DB_FRIENDS, pbdb::db_friends>::save(0, apair.second);
	}
}

void init_DB_TESTLUA()
{
	std::map<ngl::i64_actorid, pbdb::db_testlua> lmap;
	for (int i = 0; i < 10; ++i)
	{
		ngl::i64_actorid lactor1 = ngl::nguid::make(ngl::ACTOR_TESTLUA, tab_self_area, i);
		pbdb::db_testlua& ltestlua = lmap[lactor1];
		ltestlua.set_mid(lactor1);
		for (int j = 0; j < 10; ++j)
		{
			ltestlua.add_mvalue(i * 1000 + i);
		}
		pbdb::db_testlua::luadata ltemp;
		for (int i = 0; i < 5; ++i)
		{
			ltemp.set_mkey(std::format("key_{}", i).c_str());
			ltemp.set_mval(std::format("val_{}", i).c_str());
			(*ltestlua.mutable_mdatas())[i] = ltemp;
		}
	}

	for (const auto& apair : lmap)
	{
		ngl::ndbtab<pbdb::ENUM_DB_TESTLUA, pbdb::db_testlua>::save(0, apair.second);
	}
}

bool start_db(int argc, char** argv)
{
	ngl::log_error()->print("[{}] start", "DB");

	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::db_pool::instance().init(nconfig.db());
	ngl::tdb::tdb_init(false);
	ngl::db_manage::init();

	ngl::actor_gmclient::instance();

	ngl::actor_client::instance().actor_server_register();

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
			init_DB_TESTLUA();
		}
	}
	return true;
}

bool start_crossdb()
{
	ngl::log_error()->print("[{}] start", "CROSSDB");
	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}
	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::db_pool::instance().init(nconfig.crossdb());
	ngl::tdb::tcrossdb_init(false);

	ngl::actor_gmclient::instance();

	ngl::actor_client::instance().actor_server_register();
	return true;
}

bool start_world()
{
	ngl::log_error()->print("[{}] start", "WORLD");

	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_events_logic::instance();
	ngl::actor_gm::instance();
	ngl::actor_mail::instance();
	ngl::actor_chat::instance();
	ngl::actor_brief::instance();
	ngl::actor_notice::instance();
	ngl::actor_keyvalue::instance();
	ngl::actor_ranklist::instance();
	ngl::actor_activity_manage::instance();
	ngl::actor_family::instance();
	ngl::actor_friends::instance();
	ngl::actor_gmclient::instance();
	ngl::actor_example_match::instance();
	ngl::actor_example_manage::instance();
	ngl::actor_testlua::instance();
	ngl::actor_testlua2::instance();

	ngl::actor_client::instance().actor_server_register();
	return true;
}

bool start_login()
{
	ngl::log_error()->print("[{}] start", "LOGIN");

	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_login::instance();
	ngl::actor_gmclient::instance();

	ngl::actor_client::instance().actor_server_register();
	return true;
}

bool start_gateway()
{
	ngl::log_error()->print("[{}] start", "GATEWAY");

	if (!init_server(nconfig.nodeid(), { pbnet::KCP_GATEWAY }))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_gateway::instance();
	ngl::actor_gateway_g2c::instance();
	ngl::actor_gateway_c2g::instance();
	ngl::actor_gmclient::instance();
	ngl::actor_kcp::instance();

	ngl::actor_client::instance().actor_server_register();
	return true;
}

bool start_log()
{
	ngl::log_error()->print("[{}] start", "LOG");

	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() >= ngl::ELOG_MAX || ngl::sysconfig::logwritelevel() <= ngl::ELOG_NONE)
	{
		ngl::tools::no_core_dump();
	}

	int32_t llogtype = ngl::ELOG_DEFAULT | ngl::ELOG_BI;
	ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	ngl::actor_gmclient::instance();

	ngl::actor_client::instance().actor_server_register();
	return true;
}

bool start_actor()
{
	ngl::log_error()->print("[{}] start", "ACTORSERVER");

	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}

	ngl::actor_server::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_gmclient::instance();

	return true;
}

bool start_game()
{
	ngl::log_error()->print("[{}] start", "GAME");

	if (!init_server(nconfig.nodeid(), { pbnet::KCP_ROLE }))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_role_manage::instance();
	ngl::actor_create::instance();
	ngl::actor_gmclient::instance();
	ngl::actor_kcp::instance();

	ngl::actor_client::instance().actor_server_register();
	return true;
}

bool start_cross()
{
	ngl::log_error()->print("[{}] start", "CROSS");

	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_chat::instance();
	ngl::actor_ranklist::instance();
	ngl::actor_gmclient::instance();

	ngl::actor_client::instance().actor_server_register();
	return true;
}

bool start_pushserverconfig()
{
	// 将服务器配置上传lbgmsys
	ngl::xarg_info* lpublicxml = nconfig.info();
	std::string lgmurl;
	if (!lpublicxml->find("gmurl", lgmurl))
	{
		return false;
	}
	std::string lpushserver;
	if (!lpublicxml->find("push_server_config", lpushserver))
	{
		return false;
	}
	lpushserver = lgmurl + "/" + lpushserver;

	ngl::ttab_servers::instance().foreach_server([&lpushserver](ngl::tab_servers* aserver)
		{
			auto lhttp = ngl::ncurl::make_http();
			ngl::ncurl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
			ngl::ncurl::set_type(lhttp, ngl::ENUM_TYPE_GET);
			ngl::ncurl::set_url(lhttp, lpushserver);

			std::stringstream lstream;
			lstream
				<< "id=" << aserver->m_id
				<< "&name=" << aserver->m_name
				<< "&area=" << aserver->m_area
				<< "&type=" << (int32_t)aserver->m_type;

			//  ENET_TCP = 0,
			//	ENET_WS = 1,
			//	ENET_KCP = 2,
			ngl::ncjson lwrite;
			std::array<std::string, ngl::ENET_COUNT> lparm = { "tcp","ws","kcp" };
			for (ngl::net_works& item : aserver->m_net)
			{
				ngl::ncjson lwritetemp;
				ngl::njson::push(lwritetemp.json()
					, { "ip","nip","port" }, item.m_ip, item.m_nip, item.m_port
				);
				ngl::njson::push(lwrite.json(), { lparm[item.m_type].c_str() }, lwritetemp);
			}

			lwrite.set_nonformatstr(true);
			std::string lnet = lwrite.str();

			lstream << "&net=" << lnet;

			std::cout << lnet << std::endl;

			std::string lstr = lstream.str();
			ngl::ncurl::set_param(lhttp, lstr);

			ngl::ncurl::set_callback(lhttp, [lstr](int, ngl::http_parm& ahttp)
				{
					ngl::log_error()->print("[{}]->[{}]", lstr, ahttp.m_recvdata);
				});
			ngl::ncurl::send(lhttp);
		});
	return true;
}

bool start_csvserver()
{
	ngl::log_error()->print("[{}] start", "RELOADCSV");

	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_csvserver::instance();
	ngl::actor_gmclient::instance();

	ngl::actor_client::instance().actor_server_register();
	return true;
}

std::vector<std::string> get_lines()
{
	char lbuff[4096] = { 0 };
	std::cin.getline(lbuff, 4096);
	std::string lstrbuff = lbuff;
	//[== 删除多余空格
	ngl::tools::erase_repeat(lstrbuff, ' ');
	//删除多余空格 ==]

	std::vector<std::string> lvec;
	ngl::tools::splite(lstrbuff.c_str(), " ", lvec);
	return lvec;
}

bool start_robot(int argc, char** argv)
{
	ngl::log_error()->print("[{}] start", "ROBOT");

	if (!init_server(nconfig.nodeid()))
	{
		return false;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_gmclient::instance();

	ngl::actor_robot_manage::instance();

	ngl::i32_serverid llogin = ngl::nnodeid::nodeid(ngl::ttab_servers::instance().const_tab()->m_login, 1);
	ngl::actor_robot_manage::instance().connect(llogin, [](int asession)
		{
			std::cout << "连接Login服务器成功" << std::endl;
		});
	if (argc < 4)
	{
		while (1)
		{
			std::vector<std::string> lvec = get_lines();
			ngl::actor_robot_manage::parse_command(lvec);
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
			lcmd = std::format("login {}", argv[4]);
		}
		else
		{
			lcmd = std::format("logins {} {} {}", argv[4], argv[5], argv[6]);
		}
		std::vector<std::string> lvec;
		if (ngl::tools::splite(lcmd.c_str(), " ", lvec) == false)
		{
			return false;
		}
		ngl::actor_robot_manage::parse_command(lvec);
		int lnum = 10000;

		bool ltest = false;
		std::vector<int> lms;
		std::vector<std::vector<std::string>> lcmdvec;
		std::thread lthread([&ltest, &lms, &lcmdvec]()
			{
				std::vector<std::string> lvec;
				while (true)
				{
					lvec = get_lines();
					if (lvec.empty())
					{
						continue;
					}
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
					if (lvec[0] == "notest" || lvec[0] == "NOTEST")
					{
						lms.clear();
						lcmdvec.clear();
						ltest = false;
						continue;
					}
					if (lvec[0] == "tests" || lvec[0] == "TESTS")
					{
						lms.push_back(ngl::tools::lexical_cast<int>(lvec[1].c_str()));
						lcmdvec.push_back(std::vector<std::string>());
						for (int i = 2; i < lvec.size(); ++i)
						{
							lcmdvec.rbegin()->push_back(lvec[i]);
						}
						continue;
					}
					if (lvec[0] == "start" || lvec[0] == "START")
					{
						ltest = true;
						continue;
					}
					ngl::actor_robot_manage::parse_command(lvec);
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
				ngl::actor_robot_manage::parse_command(lcmdvec2);
			}
		}
	}
	return true;
}

std::string g_nodename = "node";

namespace ngl
{
	extern void server_test();
}

int ngl_main(int argc, char** argv)
{
	ngl::server_test();
	// # 名称
	std::string lname = argv[1];

	// # 区服id
	int32_t larea = ngl::tools::lexical_cast<int32_t>(argv[2]);

	// # 区服id下功能进程的序号
	int32_t ltcount = 0;
	if (lname != "robot")
	{
		ltcount = ngl::tools::lexical_cast<int32_t>(argv[3]);
	}
	else
	{
		ltcount = ngl::tools::lexical_cast<int32_t>(argv[5]);
	}

	// # 初始化关联枚举NODE_TYPE与字符串
	nconfig.init();

	nconfig.set_server(argv[1]);

	// # 加载xml配置
	nconfig.load("./config", std::format("{}_{}", lname, ltcount));

	// # 加载csv配置
	ngl::csv_base::set_path("./csv", lname);

	const ngl::tab_servers* tab = ngl::ttab_servers::instance().const_tab(argv[1], larea);
	if (tab == nullptr)
	{
		return 0;
	}
	nconfig.set_nodeid(tab->m_id, ltcount);

	if (larea < 0)
	{
		g_nodename = std::format("node_{}__{}_{}", lname, -larea, ltcount);
	}
	else
	{
		g_nodename = std::format("node_{}_{}_{}", lname, tab->m_area, ltcount);
	}

#ifdef WIN32
	// # 设置控制台窗口名称
	SetConsoleTitle(g_nodename.c_str());
#endif
	
	switch (nconfig.nodetype())
	{
	case ngl::DB:
		start_db(argc, argv);
		break;
	case ngl::GAME:
		start_game();
		break;
	case ngl::ACTORSERVER:
		start_actor();
		break;
	case ngl::LOG:
		start_log();
		break;
	case ngl::GATEWAY:
		start_gateway();
		break;
	case ngl::LOGIN:
		start_login();
		break;
	case ngl::WORLD:
		start_world();
		break;
	case ngl::RELOADCSV:
		start_csvserver();
		break;
	case ngl::ROBOT:
		start_robot(argc, argv);
		break;
	case ngl::CROSS:
		start_cross();
		break;
	case ngl::CROSSDB:
		start_crossdb();
		break;
	case ngl::PUSHSERVERCONFIG:
		start_pushserverconfig();
		break;
	default:
		return 0;
	}

	while (1)
	{
		ngl::sleep::seconds(1);
	}
	return 0;
}

std::function<void()> dump_logic()
{
	std::string ltitle = std::format("{}", g_nodename);
	return [ltitle]()
		{
			std::cout << "dump_logic()" << std::endl;
			
			auto lparm = ngl::ncurl::make_mail();
			lparm->m_smtp = nconfig.mail().m_smtp;
			lparm->m_email = nconfig.mail().m_email;
			lparm->m_password = nconfig.mail().m_password;
			lparm->m_name = nconfig.mail().m_name;
			lparm->m_title = ltitle;
			lparm->m_content = "code dump";
			lparm->m_recvs.emplace_back(std::make_pair("348634371@qq.com", "李博QQ"));
			ngl::ncurl::sendemail(lparm);
		};
}
