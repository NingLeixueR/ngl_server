#include "server_main.h"

Dumper lDumper;

#include "rfun.h"
//#include "benchmark.h"
//
//static void BM_TestFunction(benchmark::State& state) 
//{
//	for (auto _ : state) {
//		// Testing code here
//	}
//}
//BENCHMARK(BM_TestFunction);

int main(int argc, char** argv)
{	
	std::cout <<
	typeid(pbnet::PROBUFF_NET_GET_TIME_RESPONSE).name()
		<< std::endl;
	std::cout <<
	typeid(ngl::actor_forward<pbnet::PROBUFF_NET_GET_TIME_RESPONSE, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>).name()
		<< std::endl;

	//benchmark::Initialize(&argc, argv);
	//benchmark::RunSpecifiedBenchmarks();

	if (argc <= 1)
	{
		std::cout << "node [name] [tcount]" << std::endl;
		std::cout << "[name]	tab_servers.m_name" << std::endl;
		std::cout << "[tcount]	tab_servers.m_tcount" << std::endl;
		return 0;
	}

	ngl::allcsv::load();

	nconfig::init();
	nconfig::load("config");
	int32_t larea = boost::lexical_cast<int32_t>(argv[2]);
	int32_t ltcount = boost::lexical_cast<int32_t>(argv[3]);
	ngl::tab_servers* tab = ngl::ttab_servers::tab(argv[1], larea, ltcount);
	nconfig::set_server(argv[1], tab->m_id);

	ngl::xmlprotocol::load();
	ngl::init_protobuf::initall();

	ngl::nlog::getInstance().isinitfinish() = true;

	LogLocalError("[%]", 123);
	LogLocalError("[456]");
	
	

#ifdef WIN32
	// ### 设置控制台窗口名称
	char lname[1024] = { 0x0 };
	snprintf(lname, 1024, "node_%s_%s_%s", argv[1], argv[2], argv[3]);
	SetConsoleTitle(lname);
#endif
	Dumper::m_excname = lname;

	ngl::actor_typename::getInstance();

	ngl::actor_base::start_broadcast();

	assert(nconfig::node_type() != ngl::FAIL);
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

	ngl::test_attribute();
	 
	while (1)
	{
		//LogLocalError("Test Log Server:[%]", argv[1]);
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
	return 0;
}