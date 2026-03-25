#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"

#include "actor/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"

std::string get_lines()
{
	char lbuff[4096] = { 0 };
	std::cin.getline(lbuff, 4096);
	std::string lstrbuff = lbuff;
	//[== 删除多余空格
	ngl::tools::erase_repeat(lstrbuff, ' ');
	//删除多余空格 ==]
	return lstrbuff;
}

startup_error start_robot(int argc, char** argv, int* tcp_port)
{
	ngl::log_error()->print("[{}] start", "ROBOT");

	startup_error rc = init_server(nconfig.nodeid(), {}, tcp_port);
	if (rc != startup_error::ok)
	{
		return rc;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_robot_manage::instance();

	ngl::i32_serverid llogin = ngl::nnodeid::nodeid(ngl::ttab_servers::instance().const_tab()->m_login, 1);
	std::string_view lprotocol(argv[7]);
	if (lprotocol == "ws" || lprotocol == "wss")
	{
		ngl::actor_robot_manage::instance().connect(llogin, ngl::ENET_PROTOCOL::ENET_WS, [](int asession)
			{
				std::cout << "连接Login服务器成功" << std::endl;
			}
		);
	}
	else
	{
		ngl::actor_robot_manage::instance().connect(llogin, ngl::ENET_PROTOCOL::ENET_TCP, [](int asession)
			{
				std::cout << "连接Login服务器成功" << std::endl;
			}
		);
	}
	
	if (argc < 4)
	{
		while (1)
		{
			ngl::actor_robot_manage::parse_command(get_lines());
		}
	}
	else
	{
		for (int i = 0; i < 5; ++i)
		{
			ngl::sleep::seconds(1);
			std::cout << "---------------[" << i << "]---------------" << std::endl;
		}
		std::string lcmd;
		lcmd = std::format("logins {} {} {}", argv[4], argv[5], argv[6]);
		std::vector<std::string> lvec;
		if (ngl::tools::splite(lcmd.c_str(), " ", lvec) == false)
		{
			return startup_error::node_start_failed;
		}
		ngl::actor_robot_manage::parse_command(lvec);
		int lnum = 10000;

		bool ltest = false;
		std::vector<int> lms;
		std::vector<std::vector<std::string>> lcmdvec;
		std::thread lthread([&ltest, &lms, &lcmdvec]()
			{
				std::vector<std::string> lvec;
				while (true)
				{
					lvec = get_lines();
					if (lvec.empty())
					{
						continue;
					}
					if (lvec[0] == "test" || lvec[0] == "TEST")
					{
						lms.clear();
						lms.push_back(ngl::tools::lexical_cast<int>(lvec[1].c_str()));
						lcmdvec.clear();
						lcmdvec.push_back(std::vector<std::string>());
						for (int i = 2; i < lvec.size(); ++i)
						{
							lcmdvec.rbegin()->push_back(lvec[i]);
						}
						ltest = true;
						continue;
					}
					if (lvec[0] == "notest" || lvec[0] == "NOTEST")
					{
						lms.clear();
						lcmdvec.clear();
						ltest = false;
						continue;
					}
					if (lvec[0] == "tests" || lvec[0] == "TESTS")
					{
						lms.push_back(ngl::tools::lexical_cast<int>(lvec[1].c_str()));
						lcmdvec.push_back(std::vector<std::string>());
						for (int i = 2; i < lvec.size(); ++i)
						{
							lcmdvec.rbegin()->push_back(lvec[i]);
						}
						continue;
					}
					if (lvec[0] == "start" || lvec[0] == "START")
					{
						ltest = true;
						continue;
					}
					ngl::actor_robot_manage::parse_command(lvec);
				}
			});

		for (int i = 0;; ++i)
		{
			if (!ltest)
			{
				ngl::sleep::seconds(1);
				continue;
			}
			for (int j = 0; j < lcmdvec.size() && j < lms.size(); ++j)
			{
				ngl::sleep::milliseconds(lms[j]);
				std::vector<std::string> lcmdvec2 = lcmdvec[j];
				ngl::actor_robot_manage::parse_command(lcmdvec2);
			}
		}
	}
	return startup_error::ok;
}
