#include "operator_file.h"
#include "init_protobuf.h"
#include "server_main.h"
#include "manage_curl.h"
#include "nactortype.h"
#include "asio_kcp.h"
#include "dumper.h"
#include "ojson.h"
#include "ijson.h"
#include "ukcp.h"
#include "rfun.h"

#include <boost/lexical_cast.hpp>

Dumper lDumper;

int main(int argc, char** argv)
{	
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
	ngl::init_protobuf::initall();
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