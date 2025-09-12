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

#include "nserialize.h"

#include "njson.h"


Dumper lDumper;

namespace ngl
{
	struct servertype
	{
		std::vector<int32_t> servertype;

		static std::vector<const char*>& parms(const char* astr = nullptr) 
		{
			static std::vector<const char*> tempvec; 
			if (astr == nullptr) 
			{
				return tempvec;
			} 
			static std::string tempstr(astr); 
			static std::atomic<bool> lregister = true; 
			if (lregister.exchange(false) && !tempstr.empty()) 
			{
				tempvec = tools::split_str(&tempstr[0], tempstr.size());
			} 
			return tempvec;
		}
		inline void json_write(ngl::njson_write& ijsn, const char* akey) 
		{
			ngl::njson_write ltemp; 
			json_write(ltemp); 
			ngl::njson::write(ijsn, akey, ltemp.nofree());
		} 
		inline void json_write(ngl::njson_write& ijsn) 
		{
			help_writejson ltemp(parms("servertype"), ijsn); 
			ltemp.fun(0, servertype);
		} 
		inline bool json_read(ngl::njson_read& ijsn, const char* akey) 
		{
			ngl::njson_read ltemp; 
			if (!ngl::njson::read(ijsn, akey, ltemp.json())) 
			{
				return false;
			} 
			return json_read(ltemp);
		} 
		inline bool json_read(ngl::njson_read& ijsn) 
		{
			help_readjson ltemp(parms("servertype"), ijsn); 
			return ltemp.fun(0, servertype);
		}
	};
}

int main(int argc, char** argv)
{
	std::string ljson = R"({"actor_name":"ACTOR_GM","operator":"get_time","data":{"servertype":["7"]}})";
	ngl::njson_read lreadff(ljson.c_str());
	ngl::servertype lservertype;
	ngl::njson::read(lreadff, "data", lservertype);


	ngl::njson_write lwrite;
	std::map<int, std::string> lmap1 = 
	{
		{1, "c1"},
		{2, "c2"},
		{3, "c3"},
	};
	std::vector<int> lvec1 = {1,3,5,6,7};
	int32_t lv11;
	bool lv12 = false;
	ngl::njson::write(lwrite, "k1", lv11, "k2", lv12, "k3", lmap1, "k4", lvec1);
	ngl::njson_read lread(lwrite.get());
	int32_t lv21 = 0;
	bool lv22 = true;
	std::map<int, std::string> lmap2;
	std::vector<int> lvec2;
	ngl::njson::read(lread, "k1", lv21, "k2", lv22, "k3", lmap2, "k4", lvec2);

	

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