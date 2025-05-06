#include "actor_manage_robot.h"
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

const std::string lerrpath = "参数错误:EXE name areaid tab_servers::tcount";

Dumper lDumper;

struct ssss
{
	int m_value;
	~ssss()
	{
		std::cout << "hello world " << m_value << std::endl;
	}
};
struct kkkk
{
	std::shared_ptr<ssss> m_1;
	~kkkk()
	{}
};
int main(int argc, char** argv)
{
	kkkk* ltemp = (kkkk*)std::malloc(sizeof(kkkk));
	for (int i = 0; i < 100; ++i)
	{
		ltemp = new(ltemp)kkkk();
		ltemp->m_1 = std::make_shared<ssss>();
		ltemp->m_1->m_value = i;
		ltemp->~kkkk();
	}
	


	{
		ngl::slist<int> ls1(5, 50, 10);
		ngl::slist<int> ls2(5, 50, 10);
		for (int i = 0; i < 100; ++i)
		{
			ls1.push_back(i);
		}

		for (int i = 101; i < 200; ++i)
		{
			ls2.push_back(i);
		}

		for (int i = 0; i < 100; ++i)
		{
			if (i % 2 == 0)
			{
				std::cout << ls1.front() << std::endl;
				ls1.pop_front();
			}
		}

		for (int i = 0; i < 100; ++i)
		{
			if (i % 2 == 0)
			{
				std::cout << ls2.front() << std::endl;
				ls2.pop_front();
			}
		}

		ls1.push_front(ls2);
	}
	
	if (argc <= 3)
	{
		std::cout << lerrpath << std::endl;
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

	// # 加载xml配置
	nconfig::load("./config", std::format("{}_{}", lname, ltcount));

	// # 加载csv配置
	ngl::csvbase::set_path("./csv", lname);
	ngl::allcsv::load();

	const ngl::tab_servers* tab = ngl::ttab_servers::tab(argv[1], larea, ltcount);
	if (tab == nullptr)
	{
		return 0;
	}

	nconfig::set_server(argv[1], tab->m_id);

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
	// # 设置控制台窗口名称
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