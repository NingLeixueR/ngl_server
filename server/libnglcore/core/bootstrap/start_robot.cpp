#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"

#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"

#include <string_view>
#include <thread>
#include <vector>

static std::string make_cmd(const std::vector<std::string>& avec, int32_t abeg)
{
	std::string lcmd;
	ngl::tools::splicing(avec, " ", lcmd, abeg);
	return lcmd;
}

std::string get_lines()
{
	char lbuff[4096] = { 0 };
	std::cin.getline(lbuff, 4096);
	std::string lstrbuff = lbuff;
	ngl::tools::erase_repeat(lstrbuff, ' ');
	return lstrbuff;
}

startup_error start_robot(int aargc, char** aargv, int* atcp_port)
{
	ngl::log_error()->print("[{}] start", "ROBOT");

	startup_error lrc = init_server(nconfig.nodeid(), {}, atcp_port);
	if (lrc != startup_error::ok)
	{
		return lrc;
	}

	ngl::actor_client::instance();

	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}

	ngl::actor_robot_manage::instance();

	const ngl::i32_serverid llogin = ngl::nnodeid::nodeid(
		static_cast<int16_t>(ngl::ttab_servers::instance().const_tab()->m_login), 1);
	const std::string_view lprotocol =
		(aargc > 7 && aargv[7] != nullptr) ? std::string_view(aargv[7]) : std::string_view("tcp");
	if (lprotocol == "ws" || lprotocol == "wss")
	{
		ngl::actor_robot_manage::instance().connect(llogin, ngl::ENET_PROTOCOL::ENET_WS, []([[maybe_unused]] int asession)
			{
				std::cout << "connect login ok" << std::endl;
			}
		);
	}
	else
	{
		ngl::actor_robot_manage::instance().connect(llogin, ngl::ENET_PROTOCOL::ENET_TCP, []([[maybe_unused]] int asession)
			{
				std::cout << "connect login ok" << std::endl;
			}
		);
	}

	if (aargc < 7)
	{
		while (1)
		{
			const std::string lline = get_lines();
			if (!lline.empty())
			{
				ngl::actor_robot_manage::parse_command(lline);
			}
		}
	}
	else
	{
		for (int li = 0; li < 5; ++li)
		{
			ngl::sleep::seconds(1);
			std::cout << "---------------[" << li << "]---------------" << std::endl;
		}
		const std::string lcmd = std::format("logins {} {} {} {}", aargv[4], aargv[5], aargv[6], aargv[7]);
		ngl::actor_robot_manage::parse_command(lcmd);

		bool ltest = false;
		std::vector<int> lms;
		std::vector<std::string> lcmds;
		std::thread lthread([&ltest, &lms, &lcmds]()
			{
				while (true)
				{
					const std::string lline = get_lines();
					if (lline.empty())
					{
						continue;
					}

					std::vector<std::string> lvec;
					if (!ngl::tools::splite(lline.c_str(), " ", lvec) || lvec.empty())
					{
						continue;
					}

					if (lvec[0] == "test" || lvec[0] == "TEST")
					{
						if (lvec.size() < 3)
						{
							continue;
						}
						lms.clear();
						lms.push_back(ngl::tools::lexical_cast<int>(lvec[1].c_str()));
						lcmds.clear();
						lcmds.push_back(make_cmd(lvec, 2));
						ltest = true;
						continue;
					}
					if (lvec[0] == "notest" || lvec[0] == "NOTEST")
					{
						lms.clear();
						lcmds.clear();
						ltest = false;
						continue;
					}
					if (lvec[0] == "tests" || lvec[0] == "TESTS")
					{
						if (lvec.size() < 3)
						{
							continue;
						}
						lms.push_back(ngl::tools::lexical_cast<int>(lvec[1].c_str()));
						lcmds.push_back(make_cmd(lvec, 2));
						continue;
					}
					if (lvec[0] == "start" || lvec[0] == "START")
					{
						ltest = true;
						continue;
					}
					ngl::actor_robot_manage::parse_command(lline);
				}
			});
		lthread.detach();

		while (true)
		{
			if (!ltest)
			{
				ngl::sleep::seconds(1);
				continue;
			}
			for (std::size_t li = 0; li < lcmds.size() && li < lms.size(); ++li)
			{
				ngl::sleep::milliseconds(lms[li]);
				ngl::actor_robot_manage::parse_command(lcmds[li]);
			}
		}
	}
	return startup_error::ok;
}
