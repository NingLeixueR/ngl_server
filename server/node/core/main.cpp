#include "operator_file.h"
#include "template_arg.h"
#include "server_main.h"
#include "manage_curl.h"
#include "define_json.h"
#include "nactortype.h"
#include "tprotocol.h"
#include "asio_kcp.h"
#include "dumper.h"
#include "event.h"
#include "tools.h"
#include "ukcp.h"
#include "nlog.h"

#include <queue>

Dumper lDumper;

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		std::cout << "参数错误" << std::endl;
		return 0;
	}

	nconfig::init();
	// # 加载xml配置
	nconfig::load("config");
	// # 加载csv配置
	ngl::allcsv::load();

	// # 用参数 不用配置中的zoneid
	int32_t larea	= ngl::tools::lexical_cast<int32_t>(argv[2]);
	int32_t ltcount = ngl::tools::lexical_cast<int32_t>(argv[3]);
	ngl::tab_servers* tab = ngl::ttab_servers::tab(argv[1], larea, ltcount);
	if (tab == nullptr)
		return 0;
	nconfig::set_server(argv[1], tab->m_id);

	char lname[1024] = { 0x0 };
	snprintf(lname, 1024, "node_%s_%s_%s", argv[1], argv[2], argv[3]);

#ifdef WIN32
	// # 设置控制台窗口名称
	SetConsoleTitle(lname);
#endif

	Dumper::m_excname = lname;

	switch (nconfig::node_type())
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