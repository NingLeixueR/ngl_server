#include "operator_file.h"
#include "server_main.h"
#include "manage_curl.h"
#include "nactortype.h"
#include "initproto.h"
#include "asio_kcp.h"
#include "dumper.h"
#include "ojson.h"
#include "ijson.h"
#include "ukcp.h"
#include "rfun.h"

#include <boost/lexical_cast.hpp>

Dumper lDumper;

struct abc
{
	int m_1;
	float m_2;
	std::string m_3;
	std::vector<int32_t> m_4;
	std::map<int32_t, float> m_5;

	jsonaddfunc( "_1", m_1, "_2", m_2, "_3", m_3, "_4", m_4,"_5", m_5)
	
	//jsondecfunc({ "_1","_2","_3","_4","_5" }, m_1, m_2, m_3, m_4, m_5)

	inline bool dec(ngl::ojson& ijsn, const char* akey)
	{
		ngl::ojson ltemp;
		if (ijsn.dec(akey, ltemp) == false)
			return false;
		return dec(ltemp);
	}

	inline bool dec(ngl::ojson& ijsn)
	{
		return ijsn.dec({ "_1","_2","_3","_4","_5" }, 0, m_1, m_2, m_3, m_4, m_5);
	}
	
};
#include "manage_curl.h"

int main(int argc, char** argv)
{	
	/*ngl::test_manage_curl();
	while (1)
	{
		ngl::sleep::seconds(1);
	}*/
	ngl::ijson lijson;
	std::vector<int32_t> lv;
	for (int i = 0; i < 10; ++i)
		lv.push_back(i);
	lijson.add("test", lv);

	abc ltemp;
	ltemp.m_1 = 1989;
	ltemp.m_2 = 123.3;
	ltemp.m_3 = "chinachina";
	ltemp.m_4.push_back(2);
	ltemp.m_4.push_back(3);
	ltemp.m_5.insert(std::make_pair(1, 1.1));
	ltemp.m_5.insert(std::make_pair(2, 2.1));

	ltemp.add(lijson, "kkk");

	std::vector<abc> lvecabc;
	lvecabc.push_back(ltemp);
	ltemp.m_1 += 1000;
	lvecabc.push_back(ltemp);
	lijson.add("kkk2", lvecabc);

	std::string ljson;
	lijson.get(ljson);

	std::cout << std::endl;

	ngl::ojson lojson(ljson.c_str());
	std::vector<int32_t> lv2;
	lojson.dec("test", lv2);
	abc ltemp2;
	ltemp2.dec(lojson, "kkk");
	std::vector<abc> lvecabc2;
	lojson.dec("kkk2", lvecabc2);
	//cJSON_CreateArray*/


	ngl::allcsv::load();
	nconfig::init();
	nconfig::load("config");

	if (argc <= 1)
	{
		std::cout << "参数错误" << std::endl;
		return 0;
	}

	// 用参数 不用配置中的zoneid
	int32_t larea = boost::lexical_cast<int32_t>(argv[2]);
	int32_t ltcount = boost::lexical_cast<int32_t>(argv[3]);
	ngl::tab_servers* tab = ngl::ttab_servers::tab(argv[1], larea, ltcount);
	nconfig::set_server(argv[1], tab->m_id);

	ngl::init_nactor_type();
	ngl::xmlprotocol::load();
	ngl::initproto::initall();
	ngl::nlog::getInstance().isinitfinish() = true;

	char lname[1024] = { 0x0 };
	snprintf(lname, 1024, "node_%s_%s_%s", argv[1], argv[2], argv[3]);

#ifdef WIN32
	// ### 设置控制台窗口名称
	SetConsoleTitle(lname);
#endif
	Dumper::m_excname = lname;

	ngl::nactortype::getInstance();
	ngl::actor_base::start_broadcast();

	assert(nconfig::node_type() != ngl::FAIL);

	//test_s(argc, argv);


	if (nconfig::node_type() == ngl::DB)
	{
		start_db(argc, argv);
	}
	else if (nconfig::node_type() == ngl::GAME)
	{
		start_game();
	}
	else if (nconfig::node_type() == ngl::ACTORSERVER)
	{
		start_actor();
	}
	else if (nconfig::node_type() == ngl::LOG)
	{
		start_log();		
	}
	else if (nconfig::node_type() == ngl::GATEWAY)
	{
		start_gateway();
	}
	else if (nconfig::node_type() == ngl::LOGIN)
	{
		start_login();
	}
	else if (nconfig::node_type() == ngl::WORLD)
	{
		start_world();
	}
	else if (nconfig::node_type() == ngl::RELOADCSV)
	{
		start_reloadcsv();
	}
	else if (nconfig::node_type() == ngl::ROBOT)
	{
		start_robot(argc, argv);
	}
	else if (nconfig::node_type() == ngl::CROSS)
	{
		start_cross();
	}
	else if (nconfig::node_type() == ngl::PUSHSERVERCONFIG)
	{
		start_pushserverconfig();
	}

	ngl::test_attribute();
	 
	while (1)
	{
		ngl::sleep::seconds(1);
	}
	return 0;
}