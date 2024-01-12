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

template <typename T>
class abc
{};

class bcd1 {};
class bcd2 {};

int main(int argc, char** argv)
{	
	std::cout << __FILE__ << std::endl;
	constexpr std::string_view str = __FILE__;
	constexpr auto pos = str.rfind("/");
	if constexpr (pos != std::string_view::npos)
	{
		constexpr std::string_view str2 = str.substr(pos + 1);
		std::cout << str2 << std::endl;
	}



	std::cout << GM::PROBUFF_GM_DEL_NOTICE().GetDescriptor()->name() << std::endl;


	using TTT = ngl::actor_forward<GM::PROBUFF_GM_DEL_NOTICE, EPROTOCOL_TYPE_CUSTOM, true, ngl::forward>;
	std::cout << "boost name:" <<
		boost::typeindex::type_id_with_cvr<TTT>().name()
		<< std::endl;
	std::cout << "boost raw_name:" <<
		boost::typeindex::type_id_with_cvr<TTT>().raw_name()
		<< std::endl;
 	std::cout << "boost pretty_name:" <<
		boost::typeindex::type_id_with_cvr<TTT>().pretty_name()
		<< std::endl;
	
	std::cout << "std name:" <<
		typeid(TTT).name()
		<< std::endl;

	//std::cout << "std raw_name:" <<
	//	typeid(TTT).raw_name()
	//	<< std::endl;

	//std::cout <<
	//typeid(ngl::actor_forward<pbnet::PROBUFF_NET_GET_TIME_RESPONSE, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>).name()
	//	<< std::endl;
	//return 0;

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

	ngl::init_actor_type();

	ngl::xmlprotocol::load();
	ngl::init_protobuf::initall();

	std::string lnameksp = "ngl::actor_forward<pbnet::PROBUFF_NET_HEARTBEAT, (EPROTOCOL_TYPE)1, true, ngl::forward>";

	int32_t lprotocol = ngl::xmlprotocol::protocol(lnameksp);
	if (lprotocol == -1)
	{
		std::cout << "\n################[ngl::actor_forward<pbnet::PROBUFF_NET_HEARTBEAT, (EPROTOCOL_TYPE)1, true, ngl::forward>]###################" << std::endl;
		return 0;
	}


	ngl::nlog::getInstance().isinitfinish() = true;

	LogLocalError("[%]", 123);
	LogLocalError("[456]");
	

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