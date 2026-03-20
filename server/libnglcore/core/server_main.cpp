// File overview: Implements logic for core.

#include "tools/tab/xml/xml_serialize.h"
#include "server_main.h"
#include "startup_support.h"
#include "init_server.h"
#include "runtime_helpers.h"
#include "tools/arg_options.h"

#include <atomic>
#include <cctype>
#include <ctime>
#include <mutex>

#define DEF_COUNT (2000)

namespace ngl_runtime::detail
{
	struct node_bootstrap_options
	{
		// Optional node-specific startup toggles used by start_node().
		std::set<pbnet::ENUM_KCP> m_kcp_types;
		bool m_use_actor_client = true;
		bool m_create_default_log_actor = true;
		bool m_register_actor_server = true;
	};

	struct robot_test_plan
	{
		// Parallel arrays: each delay is followed by one raw command line.
		std::vector<int> m_interval_ms;
		std::vector<std::string> m_commands;
	};

	struct robot_test_state
	{
		// Console thread updates the plan while the main loop replays it.
		std::atomic_bool m_enabled = false;
		std::mutex m_mutex;
		robot_test_plan m_plan;
	};

	node_bootstrap_options make_node_options(
		std::initializer_list<pbnet::ENUM_KCP> kcp_types = {},
		bool use_actor_client = true,
		bool create_default_log_actor = true,
		bool register_actor_server = true)
	{
		node_bootstrap_options options;
		options.m_kcp_types.insert(kcp_types.begin(), kcp_types.end());
		options.m_use_actor_client = use_actor_client;
		options.m_create_default_log_actor = create_default_log_actor;
		options.m_register_actor_server = register_actor_server;
		return options;
	}

	template <pbdb::ENUM_DB TDbType, typename TRecord>
	void seed_db_record(const TRecord& record)
	{
		// Keep the seed helpers backend-agnostic so tests and bootstrap scripts work with
		// either MySQL or PostgreSQL.
		if (nconfig.dbedb() == ngl::xarg_db::edb_mysql)
		{
			ngl::ndbtab<TDbType, TRecord, ngl::nmysql_manage, ngl::nmysql_pool>::save(0, record);
		}
		else if (nconfig.dbedb() == ngl::xarg_db::edb_postgresql)
		{
			ngl::ndbtab<TDbType, TRecord, ngl::npostgresql_manage, ngl::npostgresql_pool>::save(0, record);
		}
	}

	void create_default_log_actor()
	{
		if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
		{
			int32_t llogtype = ngl::ELOG_DEFAULT;
			ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
		}
	}

	template <typename SetupFn>
	startup_error start_node(const char* node_name, int* tcp_port, const node_bootstrap_options& options, SetupFn&& setup)
	{
		ngl::log_error()->print("[{}] start", node_name);

		// init_server wires up protocols, networking and the actor scheduler.
		startup_error rc = init_server(nconfig.nodeid(), options.m_kcp_types, tcp_port);
		if (rc != startup_error::ok)
		{
			return rc;
		}

		if (options.m_use_actor_client)
		{
			ngl::actor_client::instance();
		}

		if (options.m_create_default_log_actor)
		{
			create_default_log_actor();
		}

		setup();

		if (options.m_register_actor_server && options.m_use_actor_client)
		{
			// Once all singleton actors exist locally, publish them to the route layer.
			ngl::actor_client::instance().actor_server_register();
		}

		return startup_error::ok;
	}

	template <typename TDbConfig>
	void init_database_backend(const TDbConfig& db_config)
	{
		if (nconfig.dbedb() == ngl::xarg_db::edb_mysql)
		{
			ngl::nmysql_pool::instance().init(db_config);
			ngl::nmysql_manage::init();
		}
		else if (nconfig.dbedb() == ngl::xarg_db::edb_postgresql)
		{
			ngl::npostgresql_pool::instance().init(db_config);
			ngl::npostgresql_manage::init();
		}
	}

	bool should_seed_db_data(int argc, char** argv)
	{
		return argc >= 5 && argv != nullptr && argv[4] != nullptr && std::string_view(argv[4]) == "init";
	}

	std::string command_suffix(const std::string& line, std::size_t skip_tokens)
	{
		std::size_t lpos = 0;
		std::size_t lcount = 0;
		const std::size_t lsize = line.size();
		while (lpos < lsize && lcount < skip_tokens)
		{
			while (lpos < lsize && std::isspace(static_cast<unsigned char>(line[lpos])))
			{
				++lpos;
			}
			while (lpos < lsize && !std::isspace(static_cast<unsigned char>(line[lpos])))
			{
				++lpos;
			}
			++lcount;
		}
		while (lpos < lsize && std::isspace(static_cast<unsigned char>(line[lpos])))
		{
			++lpos;
		}
		return lpos < lsize ? line.substr(lpos) : std::string();
	}

	bool apply_robot_control_command(const std::string& line, robot_test_state& state)
	{
		const std::vector<std::string> tokens = ngl::arg_options::split_command_line(line);
		if (tokens.empty())
		{
			return true;
		}

		std::string command = tokens.front();
		ngl::tools::transform_tolower(command);

		if (command == "test" || command == "tests")
		{
			int delay_ms = 0;
			if (tokens.size() < 3 || !ngl::tools::try_lexical_cast(tokens[1], delay_ms))
			{
				return true;
			}

			const std::string replay_command = command_suffix(line, 2);
			std::scoped_lock lock(state.m_mutex);
			if (command == "test")
			{
				// Replace the replay plan with a single repeating command.
				state.m_plan.m_interval_ms = { delay_ms };
				state.m_plan.m_commands = { replay_command };
				state.m_enabled.store(true, std::memory_order_release);
			}
			else
			{
				// Append one more delayed command to the current replay plan.
				state.m_plan.m_interval_ms.push_back(delay_ms);
				state.m_plan.m_commands.push_back(replay_command);
			}
			return true;
		}

		if (command == "notest")
		{
			// Clear the replay plan entirely.
			std::scoped_lock lock(state.m_mutex);
			state.m_plan = {};
			state.m_enabled.store(false, std::memory_order_release);
			return true;
		}

		if (command == "start")
		{
			// Resume replay without modifying the stored plan.
			state.m_enabled.store(true, std::memory_order_release);
			return true;
		}

		return false;
	}

	robot_test_plan copy_robot_test_plan(robot_test_state& state)
	{
		std::scoped_lock lock(state.m_mutex);
		return state.m_plan;
	}

	std::string read_console_line()
	{
		std::string line;
		if (!std::getline(std::cin, line))
		{
			return {};
		}
		return line;
	}
}

namespace ngl_runtime
{
	robot_launch_request build_robot_launch_request(int argc, char** argv)
	{
		robot_launch_request request;
		if (argc <= 4)
		{
			// No account arguments means the robot will read commands interactively.
			request.mode = robot_launch_mode::interactive;
			return request;
		}
		if (argv == nullptr || argv[4] == nullptr)
		{
			return request;
		}

		if (argc == 5)
		{
			// One extra argument maps to the legacy "login <account>" shortcut.
			request.mode = robot_launch_mode::login;
			request.command = std::format("login {}", argv[4]);
			return request;
		}

		if (argc >= 7 && argv[5] != nullptr && argv[6] != nullptr)
		{
			// Three extra arguments map to the legacy batched login flow.
			request.mode = robot_launch_mode::logins;
			request.command = std::format("logins {} {} {}", argv[4], argv[5], argv[6]);
		}
		return request;
	}

	std::vector<std::string> split_command_line(std::string line)
	{
		return ngl::arg_options::split_command_line(line);
	}

	bool build_push_server_config_param(const ngl::tab_servers& server, std::string& param)
	{
		param.clear();

		// The endpoint expects the scalar fields as flat query params.
		ngl::ncurl::param(param, "id", server.m_id);
		ngl::ncurl::param(param, "name", ngl::tools::url_encode(server.m_name));
		ngl::ncurl::param(param, "area", server.m_area);
		ngl::ncurl::param(param, "type", static_cast<int32_t>(server.m_type));

		struct json_net
		{
			std::string m_ip;
			std::string m_nip;
			uint16_t m_port;

			DPROTOCOL(json_net, m_ip, m_nip, m_port)
		};

		constexpr std::array<const char*, ngl::ENET_COUNT> kNetNames = { "tcp", "ws", "kcp" };
		ngl::ncjson net_json;
		for (const ngl::net_works& item : server.m_net)
		{
			const auto type_index = static_cast<std::size_t>(item.m_type);
			if (type_index >= kNetNames.size())
			{
				param.clear();
				return false;
			}

			json_net net_value;
			net_value.m_ip = item.m_ip;
			net_value.m_nip = item.m_nip;
			net_value.m_port = item.m_port;
			// The network list is encoded as a small JSON object and then URL-escaped.
			ngl::njson::push(net_json, { kNetNames[type_index] }, net_value);
		}

		ngl::ncurl::param(param, "net", ngl::tools::url_encode(net_json.nonformat_str()));
		return true;
	}
}

using ngl_runtime::detail::apply_robot_control_command;
using ngl_runtime::detail::copy_robot_test_plan;
using ngl_runtime::detail::create_default_log_actor;
using ngl_runtime::detail::init_database_backend;
using ngl_runtime::detail::make_node_options;
using ngl_runtime::detail::read_console_line;
using ngl_runtime::detail::robot_test_plan;
using ngl_runtime::detail::robot_test_state;
using ngl_runtime::detail::seed_db_record;
using ngl_runtime::detail::should_seed_db_data;
using ngl_runtime::detail::start_node;

startup_error start_robot_safe(int argc, char** argv, int* tcp_port);

void init_DB_ACCOUNT(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_account ltemp;
		ltemp.set_maccount(std::string(aname) + ngl::tools::lexical_cast<std::string>(i % DEF_COUNT));
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltemp.set_mpassworld("123456");
		ltemp.set_mroleid(ltemp.mid());
		ltemp.set_marea(tab_self_area);

			seed_db_record<pbdb::ENUM_DB_ACCOUNT>(ltemp);
	}
}

void init_DB_ACCOUNT()
{
	init_DB_ACCOUNT("libo", (0 * DEF_COUNT) + 1);
	init_DB_ACCOUNT("wac", (1 * DEF_COUNT) + 1);
}

// Seed demo role records.
void init_DB_ROLE(const char* aname, int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_role ltemp;

		ngl::i64_actorid lid = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i);
		ltemp.set_mid(lid);
		pbdb::db_brief lrolebase;
		lrolebase.set_mid(ngl::nguid::make(ngl::ACTOR_BRIEF, tab_self_area, i));
		lrolebase.mutable_mbase()->set_mname(std::string(aname) + ngl::tools::lexical_cast<std::string>(i % DEF_COUNT));
		lrolebase.mutable_mbase()->set_mlv(i);
		lrolebase.mutable_mbase()->set_mmoneygold(0);
		lrolebase.mutable_mbase()->set_mmoneysilver(0);
		lrolebase.mutable_mbase()->set_mcreateutc((int32_t)ngl::localtime::gettime());

			seed_db_record<pbdb::ENUM_DB_BRIEF>(lrolebase);
			seed_db_record<pbdb::ENUM_DB_ROLE>(ltemp);
	}
}

void init_DB_ROLE()
{
	std::cout << "#########init_DB_ROLE()#############" << std::endl;
	std::string lstr = std::format("{}_zone{}_", "libo", nconfig.area());
	init_DB_ROLE(lstr.c_str(), (0 * DEF_COUNT) + 1);
	lstr = std::format("{}_zone{}_", "wac", nconfig.area());
	init_DB_ROLE(lstr.c_str(), (1 * DEF_COUNT) + 1);
}

// Seed demo bag records.
void init_DB_BAG(const char* aname, int beg)
{
	(void)aname;
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_bag ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltemp.set_mmaxid(1);

			seed_db_record<pbdb::ENUM_DB_BAG>(ltemp);
	}
}

void init_DB_BAG()
{
	init_DB_BAG("libo", (0 * DEF_COUNT) + 1);
	init_DB_BAG("wac", (1 * DEF_COUNT) + 1);
}

void init_DB_TASK(const char* aname, int beg)
{
	(void)aname;
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_task ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		auto lrundatas = ltemp.mutable_mrundatas();
		std::pair<int32_t, pbdb::db_task::data> lpair;
		lpair.first = 1;
		lpair.second.set_mtaskid(1);
		lpair.second.set_mreceiveutc(0);
		lpair.second.set_mfinshutc(0);
		auto lschedules = lpair.second.mutable_mschedules();
		auto lschedulesnode = lschedules->Add();
		lschedulesnode->set_mvalue(1);
		lschedulesnode->set_msumint(10);
		(*lrundatas)[lpair.first] = lpair.second;

			seed_db_record<pbdb::ENUM_DB_TASK>(ltemp);
	}
}

void init_DB_TASK()
{
	init_DB_TASK("libo", (0 * DEF_COUNT) + 1);
	init_DB_TASK("wac", (1 * DEF_COUNT) + 1);
}

// Seed demo mailbox records.
void init_DB_MAIL(int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_mail ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));

			seed_db_record<pbdb::ENUM_DB_MAIL>(ltemp);
	}
}

void init_DB_MAIL()
{
	init_DB_MAIL((0 * DEF_COUNT) + 1);
	init_DB_MAIL((1 * DEF_COUNT) + 1);
}

void init_DB_ROLEKEYVALUE(int beg)
{
	for (int i = beg; i < beg + DEF_COUNT; ++i)
	{
		pbdb::db_rolekeyvalue ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		(*ltemp.mutable_mdata())["test1"] = "1";
		(*ltemp.mutable_mdata())["test2"] = "2";
		(*ltemp.mutable_mdata())["test3"] = "3";

			seed_db_record<pbdb::ENUM_DB_ROLEKEYVALUE>(ltemp);
	}
}

void init_DB_ROLEKEYVALUE()
{
	init_DB_ROLEKEYVALUE((0 * DEF_COUNT) + 1);
	init_DB_ROLEKEYVALUE((1 * DEF_COUNT) + 1);
}

void init_DB_NOTICE()
{
	std::vector<std::string> lvec =
	{
		"一句话", // 1
		"一辈子", // 2
		"一生情", // 3
		"哈哈哈哈哈哈哈哈哈", // 4
		"护花使者", // 5
		"牛逼克拉斯", // 6
		"战天城问问", // 7
		"吃喝拉撒睡", // 8
		"妖植为何", // 9
		"银月32城", // 10
	};
	for (int i = 1; i < 10; ++i)
	{
		pbdb::db_notice ltemp;
		ltemp.set_mid((ngl::ttab_servers::instance().const_tab()->m_area * 100) + i);
		if (ngl::tools::to_utf8(lvec[i], lvec[i]) == false)
			continue;
			ltemp.set_mnotice(lvec[i]);
			const int32_t lnow = static_cast<int32_t>(time(nullptr));
			ltemp.set_mstarttime(lnow);
			ltemp.set_mfinishtime(lnow + 36000);
			seed_db_record<pbdb::ENUM_DB_NOTICE>(ltemp);
	}
}

void init_DB_KEYVAL()
{
	pbdb::db_keyvalue ltemp;
	ltemp.set_mid(pbdb::db_keyvalue_ekv_none);
	int32_t lnow = (int32_t)ngl::localtime::gettime();
	std::string ltempstr = std::format("{}*{}", lnow, ngl::localtime::time2str(lnow, "%y/%m/%d %H:%M:%S"));
	ltemp.set_mvalue(ltempstr);

	seed_db_record<pbdb::ENUM_DB_KEYVALUE>(ltemp);
}

void init_DB_FAMILY()
{
	for (int i = 1; i < 100; ++i)
	{
		pbdb::db_family ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_FAMILY, tab_self_area, i));
		ltemp.set_mcreateutc((int32_t)ngl::localtime::gettime());
		ltemp.set_mexp(100);
		ltemp.set_mlv(1);
		ltemp.set_mname(std::format("FLIBO{}", i));
		ltemp.set_mleader(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		*ltemp.mutable_mmember()->Add() = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i);

			seed_db_record<pbdb::ENUM_DB_FAMILY>(ltemp);
		
		pbdb::db_familyer ltempfamilyer;
		ltempfamilyer.set_mjoinutc((int32_t)ngl::localtime::gettime());
		ltempfamilyer.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		ltempfamilyer.set_mposition(pbdb::db_familyer_eposition_leader);
		ltempfamilyer.set_mlastsignutc((int32_t)ngl::localtime::gettime());

			seed_db_record<pbdb::ENUM_DB_FAMILYER>(ltempfamilyer);
	}
}

void init_DB_RANKLIST()
{
	/*for (int i = 1; i < 100; ++i)
	{
		pbdb::db_ranklist ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i));
		pbdb::rankitem lrankitem;
		lrankitem.set_mtime((int32_t)ngl::localtime::gettime());
		lrankitem.set_mvalue(i);
		(*ltemp.mutable_mitems())[(int)pbdb::eranklist::lv] = lrankitem;

			seed_db_record<pbdb::ENUM_DB_RANKLIST>(ltemp);
	}*/
}

void init_DB_FRIENDS()
{
	std::map<ngl::i64_actorid, pbdb::db_friends> lmap;
	for (int i = 0; i < 10; ++i)
	{
		int lbeg = i * 10;
		int lend = lbeg + 10;
		for (int j1 = lbeg; j1 < lend; ++j1)
		{
			ngl::i64_actorid lactor1 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, j1);
			pbdb::db_friends& lfriends = lmap[lactor1];
			lfriends.set_mid(lactor1);
			for (int j2 = lbeg; j2 < lend; ++j2)
			{
				if (j1 != j2)
				{
					ngl::i64_actorid lactor2 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, j2);
					ngl::i64_actorid lactor3 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, j2 + 10);
					lfriends.add_mfriends(lactor2);
					lfriends.add_mapplyfriends(lactor3);
				}
			}
		}
	}

	for (auto& apair : lmap)
	{
		seed_db_record<pbdb::ENUM_DB_FRIENDS>(apair.second);
	}
}

void init_DB_TESTLUA()
{
	std::map<ngl::i64_actorid, pbdb::db_testlua> lmap;
	for (int i = 0; i < 10; ++i)
	{
		ngl::i64_actorid lactor1 = ngl::nguid::make(ngl::ACTOR_TESTLUA, tab_self_area, i);
		pbdb::db_testlua& ltestlua = lmap[lactor1];
		ltestlua.set_mid(lactor1);
		for (int j = 0; j < 10; ++j)
		{
			ltestlua.add_mvalue(i * 1000 + j);
		}
		pbdb::db_testlua::luadata ltemp;
		for (int index = 0; index < 5; ++index)
		{
			ltemp.set_mkey(std::format("key_{}", index).c_str());
			ltemp.set_mval(std::format("val_{}", index).c_str());
			(*ltestlua.mutable_mdatas())[index] = ltemp;
		}
	}

	for (auto& apair : lmap)
	{
		seed_db_record<pbdb::ENUM_DB_TESTLUA>(apair.second);
	}
}

startup_error start_db(int argc, char** argv, int* tcp_port)
{
	return start_node("DB", tcp_port, make_node_options(), [argc, argv]()
		{
			init_database_backend(nconfig.db());
			ngl::tdb::tdb_init(false);
			ngl::actor_gmclient::instance();

			if (should_seed_db_data(argc, argv))
			{
				init_DB_BAG();
				init_DB_TASK();
				init_DB_ROLE();
				init_DB_NOTICE();
				init_DB_FAMILY();
				init_DB_KEYVAL();
				init_DB_ACCOUNT();
				init_DB_ROLEKEYVALUE();
				init_DB_RANKLIST();
				init_DB_FRIENDS();
				init_DB_TESTLUA();
			}
		});
}

startup_error start_crossdb(int* tcp_port)
{
	return start_node("CROSSDB", tcp_port, make_node_options(), []()
		{
			init_database_backend(nconfig.crossdb());
			ngl::tdb::tcrossdb_init(false);
			ngl::actor_gmclient::instance();
		});
}

startup_error start_world(int* tcp_port)
{
	return start_node("WORLD", tcp_port, make_node_options(), []()
		{
			ngl::actor_events_logic::instance();
			ngl::actor_gm::instance();
			ngl::actor_mail::instance();
			ngl::actor_chat::instance();
			ngl::actor_brief::instance();
			ngl::actor_notice::instance();
			ngl::actor_keyvalue::instance();
			ngl::actor_ranklist::instance();
			ngl::actor_activity_manage::instance();
			ngl::actor_family::instance();
			ngl::actor_friends::instance();
			ngl::actor_gmclient::instance();
			ngl::actor_example_match::instance();
			ngl::actor_example_manage::instance();
			ngl::actor_testlua::instance();
			ngl::actor_testlua2::instance();
		});
}

startup_error start_login(int* tcp_port)
{
	return start_node("LOGIN", tcp_port, make_node_options(), []()
		{
			ngl::actor_login::instance();
			ngl::actor_gmclient::instance();
		});
}

startup_error start_gateway(int* tcp_port)
{
	return start_node("GATEWAY", tcp_port, make_node_options({ pbnet::KCP_GATEWAY }), []()
		{
			ngl::actor_gateway::instance();
			ngl::actor_gateway_g2c::instance();
			ngl::actor_gateway_c2g::instance();
			ngl::actor_gmclient::instance();
			ngl::actor_kcp::instance();
		});
}

startup_error start_log(int* tcp_port)
{
	return start_node("LOG", tcp_port, make_node_options({}, true, false, true), []()
		{
			if (ngl::sysconfig::logwritelevel() >= ngl::ELOG_MAX || ngl::sysconfig::logwritelevel() <= ngl::ELOG_NONE)
			{
				ngl::tools::no_core_dump();
			}

			int32_t llogtype = ngl::ELOG_DEFAULT | ngl::ELOG_BI;
			ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
			ngl::actor_gmclient::instance();
		});
}

startup_error start_actor(int* tcp_port)
{
	return start_node("ACTORSERVER", tcp_port, make_node_options({}, false, false, false), []()
		{
			ngl::actor_server::instance();
			create_default_log_actor();
			ngl::actor_gmclient::instance();
		});
}

startup_error start_game(int* tcp_port)
{
	return start_node("GAME", tcp_port, make_node_options({ pbnet::KCP_ROLE }), []()
		{
			ngl::actor_role_manage::instance();
			ngl::actor_create::instance();
			ngl::actor_gmclient::instance();
			ngl::actor_kcp::instance();
		});
}

startup_error start_cross(int* tcp_port)
{
	return start_node("CROSS", tcp_port, make_node_options(), []()
		{
			ngl::actor_chat::instance();
			ngl::actor_ranklist::instance();
			ngl::actor_gmclient::instance();
		});
}

startup_error start_pushserverconfig(int* tcp_port)
{
	(void)tcp_port;
	// This process only pushes tab_servers data to the external GM service.
	ngl::xarg_info* lpublicxml = nconfig.info();
	std::string lgmurl;
	if (!lpublicxml->find("gmurl", lgmurl))
	{
		return startup_error::node_start_failed;
	}
	std::string lpushserver;
	if (!lpublicxml->find("push_server_config", lpushserver))
	{
		return startup_error::node_start_failed;
	}
	lpushserver = lgmurl + "/" + lpushserver;
	bool lhas_invalid_network = false;

	// Push every configured server entry independently so one bad row does not block logging
	// for the rest of the batch.
	ngl::ttab_servers::instance().foreach_server([&lpushserver, &lhas_invalid_network](ngl::tab_servers* aserver)
		{
			auto lhttp = ngl::ncurl::http();
			ngl::ncurl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
			ngl::ncurl::set_type(lhttp, ngl::ENUM_TYPE_GET);
			ngl::ncurl::set_url(lhttp, lpushserver);

			std::string lparam;
			if (!ngl_runtime::build_push_server_config_param(*aserver, lparam))
			{
				lhas_invalid_network = true;
				ngl::log_error()->print("[pushserverconfig] invalid network type server:{} type:{}", aserver->m_id, static_cast<int32_t>(aserver->m_type));
				return;
			}

			ngl::ncurl::set_param(lhttp, lparam);

			ngl::ncurl::set_callback(lhttp, [lparam](int, ngl::http_parm& ahttp)
				{
					ngl::log_error()->print("[{}]->[{}]", lparam, ahttp.m_recvdata);
				});
			ngl::ncurl::send(lhttp);
		});
	return lhas_invalid_network ? startup_error::node_start_failed : startup_error::ok;
}

startup_error start_csvserver(int* tcp_port)
{
	return start_node("RELOADCSV", tcp_port, make_node_options(), []()
		{
			ngl::actor_csvserver::instance();
			ngl::actor_gmclient::instance();
		});
}

startup_error start_robot(int argc, char** argv, int* tcp_port)
{
	return start_robot_safe(argc, argv, tcp_port);
#if 0
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

	ngl::i32_serverid llogin = ngl::nnodeid::nodeid(static_cast<int16_t>(ngl::ttab_servers::instance().const_tab()->m_login), 1);
	ngl::actor_robot_manage::instance().connect(llogin, ngl::ENET_TCP, [](int)
		{
			std::cout << "连接Login服务器成功" << std::endl;
		}
	);
	if (argc < 4)
	{
		while (1)
		{
			ngl::actor_robot_manage::parse_command(get_line());
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
		if (argc < 6)
		{
			lcmd = std::format("login {}", argv[4]);
		}
		else
		{
			lcmd = std::format("logins {} {} {}", argv[4], argv[5], argv[6]);
		}
		ngl::actor_robot_manage::parse_command(lcmd);
		bool ltest = false;
		std::vector<int> lms;
		std::vector<std::string> lcmdvec;
		std::thread lthread([&ltest, &lms, &lcmdvec]()
			{
				std::string lline;
				while (true)
				{
					lline = get_line();
					const std::vector<std::string> lvec = ngl_runtime::split_command_line(lline);
					if (lvec.empty())
					{
						continue;
					}
					if (lvec[0] == "test" || lvec[0] == "TEST")
					{
						lms.clear();
						lms.push_back(ngl::tools::lexical_cast<int>(lvec[1].c_str()));
						lcmdvec.clear();
						lcmdvec.push_back(ngl_runtime::detail::command_suffix(lline, 2));
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
						lcmdvec.push_back(ngl_runtime::detail::command_suffix(lline, 2));
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

		for (int i = 0;; ++i)
		{
			if (!ltest)
			{
				ngl::sleep::seconds(1);
				continue;
			}
			for (std::size_t j = 0; j < lcmdvec.size() && j < lms.size(); ++j)
			{
				ngl::sleep::milliseconds(lms[j]);
				ngl::actor_robot_manage::parse_command(lcmdvec[j]);
			}
		}
	}
#endif
}

startup_error start_robot_safe(int argc, char** argv, int* tcp_port)
{
	const ngl_runtime::robot_launch_request request = ngl_runtime::build_robot_launch_request(argc, argv);
	if (request.mode == ngl_runtime::robot_launch_mode::invalid)
	{
		return startup_error::invalid_args;
	}

	std::string bootstrap_command;
	if (request.mode != ngl_runtime::robot_launch_mode::interactive)
	{
		bootstrap_command = request.command;
		if (bootstrap_command.empty())
		{
			return startup_error::node_start_failed;
		}
	}

	return start_node("ROBOT", tcp_port, make_node_options(), [request, bootstrap_command = std::move(bootstrap_command)]() mutable
		{
			ngl::actor_robot_manage::instance();

			ngl::i32_serverid llogin = ngl::nnodeid::nodeid(static_cast<int16_t>(ngl::ttab_servers::instance().const_tab()->m_login), 1);
				ngl::actor_robot_manage::instance().connect(llogin, ngl::ENET_TCP, [](int)
					{
						std::cout << "connected login server" << std::endl;
					}
				);

			if (request.mode == ngl_runtime::robot_launch_mode::interactive)
			{
				// Pure interactive mode behaves like the legacy robot shell.
				while (true)
				{
					std::string line = read_console_line();
					if (!line.empty())
					{
						ngl::actor_robot_manage::parse_command(std::move(line));
					}
				}
			}

			for (int i = 0; i < 5; ++i)
			{
				ngl::sleep::seconds(1);
				std::cout << "---------------[" << i << "]---------------" << std::endl;
			}

			ngl::actor_robot_manage::parse_command(std::move(bootstrap_command));

			robot_test_state state;
			std::thread([&state]()
				{
					// Console input can either mutate the replay plan or execute a normal command immediately.
					while (true)
					{
						std::string line = read_console_line();
						if (apply_robot_control_command(line, state))
						{
							continue;
						}
						if (!line.empty())
						{
							ngl::actor_robot_manage::parse_command(std::move(line));
						}
					}
				}).detach();

			for (;;)
			{
				if (!state.m_enabled.load(std::memory_order_acquire))
				{
					ngl::sleep::seconds(1);
					continue;
				}

				// Snapshot the replay plan so the console thread can keep editing the next iteration.
				const robot_test_plan plan = copy_robot_test_plan(state);
				if (plan.m_interval_ms.empty() || plan.m_commands.empty())
				{
					ngl::sleep::seconds(1);
					continue;
				}

				for (std::size_t i = 0; i < plan.m_commands.size() && i < plan.m_interval_ms.size(); ++i)
				{
					ngl::sleep::milliseconds(plan.m_interval_ms[i]);
					ngl::actor_robot_manage::parse_command(plan.m_commands[i]);
				}
			}
		});
}

int ngl_main(int argc, char** argv)
{
	ngl::arg_options loptions("test");
	
	loptions.init_help(
		"--help --h  帮助\n"
		"--input -i 输入\n"
	);
	loptions.init_options<int32_t>("input,i", "输入"
		, 0
	);
	loptions.positional("input", 1);

	{
		loptions.parse("--help --input 123");
		int32_t linput = 11111;

		if (loptions.value("input", linput))
		{
			std::cout << "ok" << std::endl;
		}
		std::string lhelp;
		if (loptions.value("help", lhelp))
		{
			std::cout << "ok" << std::endl;
		}
	}
	




	ngl_startup::context ctx{};
	const ngl_startup::prepare_result lprepare = ngl_startup::prepare_context(argc, argv, ctx);
	if (lprepare.code != startup_error::ok)
	{
		ngl_startup::log_failure(lprepare.code, ctx, lprepare.reason);
		return static_cast<int>(lprepare.code);
	}

#ifdef WIN32
	SetConsoleTitle(nconfig.servername().c_str());
#endif

	startup_error rc = startup_error::ok;
	// Each node type shares the same bootstrap path but installs a different actor set.
	switch (nconfig.nodetype())
	{
	case ngl::DB:
		rc = start_db(argc, argv, &ctx.tcp_port);
		break;
	case ngl::GAME:
		rc = start_game(&ctx.tcp_port);
		break;
	case ngl::ACTORSERVER:
		rc = start_actor(&ctx.tcp_port);
		break;
	case ngl::LOG:
		rc = start_log(&ctx.tcp_port);
		break;
	case ngl::GATEWAY:
		rc = start_gateway(&ctx.tcp_port);
		break;
	case ngl::LOGIN:
		rc = start_login(&ctx.tcp_port);
		break;
	case ngl::WORLD:
		rc = start_world(&ctx.tcp_port);
		break;
	case ngl::RELOADCSV:
		rc = start_csvserver(&ctx.tcp_port);
		break;
	case ngl::ROBOT:
		rc = start_robot_safe(argc, argv, &ctx.tcp_port);
		break;
	case ngl::CROSS:
		rc = start_cross(&ctx.tcp_port);
		break;
	case ngl::CROSSDB:
		rc = start_crossdb(&ctx.tcp_port);
		break;
	case ngl::PUSHSERVERCONFIG:
		rc = start_pushserverconfig(&ctx.tcp_port);
		break;
	default:
		rc = startup_error::invalid_node_type;
		break;
	}

	if (rc != startup_error::ok)
	{
		ngl_startup::log_failure(rc, ctx, "node start failed");
		return static_cast<int>(rc);
	}

	while (1)
	{
		ngl::sleep::seconds(1);
	}
	return static_cast<int>(startup_error::ok);
}


std::function<void()> dump_logic(const std::string& acontent)
{
	return ngl::tools::send_mail(acontent);
}
