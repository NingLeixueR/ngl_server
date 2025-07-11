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

Dumper lDumper;

struct aaaaaaaa
{
	/*********************************/
	int32_t                          m_id; // [index:0][load:y] id 
	std::string                      m_name; // [index:1][load:y] ���� 
	//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int16_t                          m_area; // [index:3][load:y] ����
	/*********************************/
	// ���л������л����
	dprotocoljson(aaaaaaaa, m_id, m_name, m_area)
};

int main(int argc, char** argv)
{
	std::map<int, float>::key_type;
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
	std::string ljson;
	ngl::json_write ljwrite;
	aaaaaaaa laaaaaaaa;
	laaaaaaaa.m_id = 1;
	laaaaaaaa.m_name = "china";
	laaaaaaaa.m_area = 3;
	laaaaaaaa.write(ljwrite);
	ljwrite.get(ljson);
	

	/*ngl::tmapjson<EPROTOCOL_TYPE_CUSTOM, std::map<int, typename ngl::ttab_servers::type_tab>> ltemp(ngl::ttab_servers::instance().tablecsv());
	std::string ljson;
	ngl::json_write ljwrite;
	ltemp.write(ljwrite);
	ljwrite.get(ljson);*/


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