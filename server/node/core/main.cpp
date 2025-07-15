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
	std::string                      m_name; // [index:1][load:y] 名字 
	//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int16_t                          m_area; // [index:3][load:y] 区服
	/*********************************/
	// 序列化反序列化相关
	dprotocoljson(aaaaaaaa, m_id, m_name, m_area)
};

int main(int argc, char** argv)
{
	std::map<int, float>::key_type;
	if (argc <= 3)
	{
		std::cout << "参数错误:EXE name areaid tab_servers::tcount" << std::endl;
		return 0;
	}

	// # 名称
	std::string lname = argv[1];

	// # 区服id
	int32_t larea = ngl::tools::lexical_cast<int32_t>(argv[2]);

	// # 区服id下功能进程的序号
	int32_t ltcount = ngl::tools::lexical_cast<int32_t>(argv[3]);

	// # 初始化关联枚举NODE_TYPE与字符串
	nconfig::init();

	nconfig::set_server(argv[1]);

	// # 加载xml配置
	nconfig::load("./config", std::format("{}_{}", lname, ltcount));

	// # 加载csv配置
	ngl::csvbase::set_path("./csv", lname);

	const ngl::tab_servers* tab = ngl::ttab_servers::instance().tab(argv[1], larea, ltcount);
	if (tab == nullptr)
	{
		return 0;
	}

	std::map<std::string, pbdb::db_account> lmap;
	for (int i = 1;i<10; ++i)
	{
		pbdb::db_account ltemp;
		ltemp.set_mid(i);
		ltemp.set_maccount(std::format("libo{}",i));
		ltemp.set_marea(i + 10);
		ltemp.set_mpassworld(std::format("ribenrensb{}", i));
		ltemp.set_mroleid(1000 + i);
		lmap[ngl::tools::lexical_cast<std::string>(i)] = ltemp;
	}
	std::string ljsontemp;
	ngl::tools::proto2json(lmap, ljsontemp);

	std::map<std::string, pbdb::db_account> lmap2;
	ngl::tools::json2proto(ljsontemp, lmap2);


	//ngl::tmapjson<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::db_account> ltempkk1(lmap2);
	//ngl::json_write lwritekk1;
	//ltempkk1.write(lwritekk1);
	//std::string ljsonkk1;
	//lwritekk1.get(ljsonkk1);

	//std::map<std::string, pbdb::db_account> lmap3;
	//ngl::tmapjson<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::db_account> ltempkk2(lmap3);
	//ngl::json_read lreadkk1;
	//ltempkk2.read(lreadkk1);


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
	// # 设置控制台窗口名称
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