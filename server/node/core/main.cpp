#include "actor_robot_manage.h"
#include "operator_file.h"
#include "template_arg.h"
#include "server_main.h"
#include "manage_curl.h"
#include "nactortype.h"
#include "tprotocol.h"
#include "asio_kcp.h"
#include "dumper.h"
#include "tools.h"
#include "ukcp.h"
#include "nlog.h"
#include "slist.h"
#include "tools.h"


#include "lua.hpp"

Dumper lDumper;
struct nscript_outdata
{
	std::vector<std::string> m_vecdata;
	def_jsonfunction("m_vecdata", m_vecdata)
};

int main(int argc, char** argv)
{
	ngl::nscriptlua lnscriptlua;
	lnscriptlua.init("test.lua");
	lnscriptlua.push_data("kkk", 1, "k1");
	lnscriptlua.push_data("kkk", 2, "k2");
	lnscriptlua.push_data("kkk", 3, "k3");
	lnscriptlua.handle("1234567");
	std::string kkkkkkkkkkkk;
	lnscriptlua.check_outdata("kkk", ngl::nguid::make(), kkkkkkkkkkkk);

	nscript_outdata ltemp;
	ltemp.m_vecdata = {"123", "456", "789"};
	ngl::json_write lwrite;
	ltemp.write(lwrite);
	std::string lstrtt;
	lwrite.get(lstrtt);

	if (argc <= 3)
	{
		std::cout << "��������:EXE name areaid tab_servers::tcount" << std::endl;
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
		lnodename = std::format("node_{}_{}_{}", lname, tab->m_area, ltcount);
	}

#ifdef WIN32
	// # ���ÿ���̨��������
	SetConsoleTitle(lnodename.c_str());
#endif

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