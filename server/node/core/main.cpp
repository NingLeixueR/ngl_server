#include "server_main.h"
#include "dumper.h"

Dumper lDumper;

#include "rfun.h"

#include "ojson.h"
#include "ijson.h"
#include "manage_curl.h"

#include <boost/lexical_cast.hpp>
#include "operator_file.h"
#include "init_protobuf.h"

#include "asio_kcp.h"
#include "udp_kcp.h"

////void test_s(int argc, char** argv)
////{
////	if (!init_server(nconfig::m_nodeid))
////		return;
////	std::string largv = argv[4];
////	if (largv == "client")
////	{
////		ngl::udp_kcp::getInstance(10000 + boost::lexical_cast<int>(argv[5]));
////		ngl::udp_kcp::getInstance().connect("127.0.0.1", 12345, [](ngl::i32_session asession)
////			{
////				pbnet::PROBUFF_NET_GET_TIME pro;
////				//pro.set_m_utc(ngl::localtime::gettime());
////				ngl::asio_udp_endpoint lendpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12345);
////				ngl::udp_kcp::getInstance().send(lendpoint, pro, -1, -1);
////			});
////		
////
////		//ngl::asio_kcp lkcp(10000+boost::lexical_cast<int>(argv[2]));
////		//
////		//ngl::asio_udp_endpoint lendpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12345);
////		//std::string lstr;
////		//for (int i = 0; i < 5000; ++i)
////		//{
////		//	lstr += 'a';
////		//}
////
////		////lkcp.send(lendpoint, lstr.c_str(), lstr.size() + 1);
////		//pbnet::PROBUFF_NET_GET_TIME_RESPONSE pro;
////		//pro.set_m_utc(ngl::localtime::gettime());
////		//lkcp.send(lendpoint, "a", 2);
////
////		//while (1)
////		//{
////		//	boost::this_thread::sleep(boost::posix_time::seconds(1));
////		//}
////	}
////	else if (largv == "server")
////	{
////		//ngl::asio_kcp lkcp(12345);
////		ngl::udp_kcp::getInstance(12345);
////
////		//while (1)
////		//{
////		//	boost::this_thread::sleep(boost::posix_time::seconds(1));
////		//}
////	}
////}


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

	ngl::init_actor_type();
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

	ngl::actor_typename::getInstance();
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
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
	return 0;
}