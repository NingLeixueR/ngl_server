#include "actor_robot_manage.h"
#include "operator_file.h"
#include "template_arg.h"
#include "server_main.h"
#include "manage_curl.h"
#include "define_json.h"
#include "nactortype.h"
#include "tprotocol.h"
#include "asio_kcp.h"
#include "dumper.h"
#include "tools.h"
#include "ukcp.h"
#include "nlog.h"
#include "slist.h"
#include <queue>

const std::string lerrpath = "��������:EXE name areaid tab_servers::tcount";

Dumper lDumper;

int main(int argc, char** argv)
{
	if (argc <= 3)
	{
		std::cout << lerrpath << std::endl;
		return 0;
	}

	// # ����
	std::string lname = argv[1];

	// # ����id
	int32_t larea = ngl::tools::lexical_cast<int32_t>(argv[2]);

	// # ����id�¹��ܽ��̵����
	int32_t ltcount = ngl::tools::lexical_cast<int32_t>(argv[3]);

	// # ��ʼ������ö��NODE_TYPE���ַ���
	nconfig::init();

	nconfig::set_server(argv[1]);

	// # ����xml����
	nconfig::load("./config", std::format("{}_{}", lname, ltcount));

	// # ����csv����
	ngl::csvbase::set_path("./csv", lname);

	const ngl::tab_servers* tab = ngl::ttab_servers::instance().tab(argv[1], larea, ltcount);
	if (tab == nullptr)
	{
		return 0;
	}

	nconfig::set_nodeid(tab->m_id);

	std::string lnodename;
	if (larea < 0)
	{
		lnodename = std::format("node_{}__{}_{}", lname, -larea, ltcount);
	}
	else
	{
		lnodename = std::format("node_{}_{}_{}", lname, larea, ltcount);
	}

#ifdef WIN32
	// # ���ÿ���̨��������
	SetConsoleTitle(lnodename.c_str());
#endif

	//std::string lmailtxt = std::format("{}:{}", lnodename, ngl::localtime::time2str("%Y-%m-%d %H:%M:%S"));
	//ngl::test_mail(lmailtxt.c_str(), lmailtxt.c_str());

	Dumper::m_excname = lnodename;
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