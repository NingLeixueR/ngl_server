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

namespace
{
	constexpr int def_count = 2000;
}

namespace ngl_runtime::detail
{
	struct node_boot_opt
	{
		// Optional node-specific startup toggles used by start_node().
		std::set<pbnet::ENUM_KCP> m_kcp_types;
		bool m_use_actor_client = true;
		bool m_make_log = true;
		bool m_reg_actor = true;
	};

	struct robot_plan
	{
		// Parallel arrays: each delay is followed by one raw command line.
		std::vector<int> m_interval_ms;
		std::vector<std::string> m_commands;
	};

	struct robot_state
	{
		// Console thread updates the plan while the main loop replays it.
		std::atomic_bool m_enabled = false;
		std::mutex m_mutex;
		robot_plan m_plan;
	};

	node_boot_opt make_node_opt(
		std::initializer_list<pbnet::ENUM_KCP> akcp_types = {},
		bool ause_actor = true,
		bool amake_log = true,
		bool areg_actor = true)
	{
		node_boot_opt lopts;
		lopts.m_kcp_types.insert(akcp_types.begin(), akcp_types.end());
		lopts.m_use_actor_client = ause_actor;
		lopts.m_make_log = amake_log;
		lopts.m_reg_actor = areg_actor;
		return lopts;
	}

	template <pbdb::ENUM_DB tdb, typename trec>
	void save_seed(const trec& arec)
	{
		// Keep the seed helpers backend-agnostic so tests and bootstrap scripts work with
		// either MySQL or PostgreSQL.
		if (nconfig.dbedb() == ngl::xarg_db::edb_mysql)
		{
			ngl::ndbtab<tdb, trec, ngl::nmysql_manage, ngl::nmysql_pool>::save(0, arec);
		}
		else if (nconfig.dbedb() == ngl::xarg_db::edb_postgresql)
		{
			ngl::ndbtab<tdb, trec, ngl::npostgresql_manage, ngl::npostgresql_pool>::save(0, arec);
		}
	}

	void make_log_actor()
	{
		if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
		{
			int32_t llogtype = ngl::ELOG_DEFAULT;
			ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
		}
	}

	template <typename tsetup>
	startup_error start_node(const char* anode_name, int* atcp_port, const node_boot_opt& aopts, tsetup&& asetup)
	{
		ngl::log_error()->print("[{}] start", anode_name);

		// init_server wires up protocols, networking and the actor scheduler.
		startup_error lrc = init_server(nconfig.nodeid(), aopts.m_kcp_types, atcp_port);
		if (lrc != startup_error::ok)
		{
			return lrc;
		}

		if (aopts.m_use_actor_client)
		{
			ngl::actor_client::instance();
		}

		if (aopts.m_make_log)
		{
			make_log_actor();
		}

		asetup();

		if (aopts.m_reg_actor && aopts.m_use_actor_client)
		{
			// Once all singleton actors exist locally, publish them to the route layer.
			ngl::actor_client::instance().actor_server_register();
		}

		return startup_error::ok;
	}

	template <typename tdb_cfg>
	void init_db_back(const tdb_cfg& adb_cfg)
	{
		if (nconfig.dbedb() == ngl::xarg_db::edb_mysql)
		{
			ngl::nmysql_pool::instance().init(adb_cfg);
			ngl::nmysql_manage::init();
		}
		else if (nconfig.dbedb() == ngl::xarg_db::edb_postgresql)
		{
			ngl::npostgresql_pool::instance().init(adb_cfg);
			ngl::npostgresql_manage::init();
		}
	}

	bool need_seed_db(int aargc, char** aargv)
	{
		return aargc >= 5 && aargv != nullptr && aargv[4] != nullptr && std::string_view(aargv[4]) == "init";
	}

	std::string command_suffix(const std::string& aline, std::size_t askip)
	{
		std::size_t lpos = 0;
		std::size_t lcount = 0;
		const std::size_t lsize = aline.size();
		while (lpos < lsize && lcount < askip)
		{
			while (lpos < lsize && std::isspace(static_cast<unsigned char>(aline[lpos])))
			{
				++lpos;
			}
			while (lpos < lsize && !std::isspace(static_cast<unsigned char>(aline[lpos])))
			{
				++lpos;
			}
			++lcount;
		}
		while (lpos < lsize && std::isspace(static_cast<unsigned char>(aline[lpos])))
		{
			++lpos;
		}
		return lpos < lsize ? aline.substr(lpos) : std::string();
	}

	bool apply_robot_cmd(const std::string& aline, robot_state& astate)
	{
		const std::vector<std::string> ltokens = ngl::arg_options::split_command_line(aline);
		if (ltokens.empty())
		{
			return true;
		}

		std::string lcmd = ltokens.front();
		ngl::tools::transform_tolower(lcmd);

		if (lcmd == "test" || lcmd == "tests")
		{
			int ldelay = 0;
			if (ltokens.size() < 3 || !ngl::tools::try_lexical_cast(ltokens[1], ldelay))
			{
				return true;
			}

			const std::string lreplay = command_suffix(aline, 2);
			std::scoped_lock llock(astate.m_mutex);
			if (lcmd == "test")
			{
				// Replace the replay plan with a single repeating command.
				astate.m_plan.m_interval_ms = { ldelay };
				astate.m_plan.m_commands = { lreplay };
				astate.m_enabled.store(true, std::memory_order_release);
			}
			else
			{
				// Append one more delayed command to the current replay plan.
				astate.m_plan.m_interval_ms.push_back(ldelay);
				astate.m_plan.m_commands.push_back(lreplay);
			}
			return true;
		}

		if (lcmd == "notest")
		{
			// Clear the replay plan entirely.
			std::scoped_lock llock(astate.m_mutex);
			astate.m_plan = {};
			astate.m_enabled.store(false, std::memory_order_release);
			return true;
		}

		if (lcmd == "start")
		{
			// Resume replay without modifying the stored plan.
			astate.m_enabled.store(true, std::memory_order_release);
			return true;
		}

		return false;
	}

	robot_plan copy_plan(robot_state& astate)
	{
		std::scoped_lock llock(astate.m_mutex);
		return astate.m_plan;
	}

	std::string read_line()
	{
		std::string lline;
		if (!std::getline(std::cin, lline))
		{
			return {};
		}
		return lline;
	}
}

namespace ngl_runtime
{
	robot_launch_request build_robot_req(int aargc, char** aargv)
	{
		robot_launch_request lreq;
		if (aargc <= 4)
		{
			// No account arguments means the robot will read commands interactively.
			lreq.mode = robot_launch_mode::interactive;
			return lreq;
		}
		if (aargv == nullptr || aargv[4] == nullptr)
		{
			return lreq;
		}

		if (aargc == 5)
		{
			// One extra argument maps to the legacy "login <account>" shortcut.
			lreq.mode = robot_launch_mode::login;
			lreq.command = std::format("login {}", aargv[4]);
			return lreq;
		}

		if (aargc >= 7 && aargv[5] != nullptr && aargv[6] != nullptr)
		{
			// Three extra arguments map to the legacy batched login flow.
			lreq.mode = robot_launch_mode::logins;
			lreq.command = std::format("logins {} {} {}", aargv[4], aargv[5], aargv[6]);
		}
		return lreq;
	}

	bool build_push_cfg(const ngl::tab_servers& asrv, std::string& aparam)
	{
		aparam.clear();
		aparam.reserve(asrv.m_name.size() + (asrv.m_net.size() * 96) + 64);

		// The endpoint expects the scalar fields as flat query params.
		ngl::ncurl::param(aparam, "id", asrv.m_id);
		ngl::ncurl::param(aparam, "name", ngl::tools::url_encode(asrv.m_name));
		ngl::ncurl::param(aparam, "area", asrv.m_area);
		ngl::ncurl::param(aparam, "type", static_cast<int32_t>(asrv.m_type));

		struct json_net
		{
			std::string m_ip;
			std::string m_nip;
			uint16_t m_port;

			DPROTOCOL(json_net, m_ip, m_nip, m_port)
		};

		constexpr std::array<const char*, ngl::ENET_COUNT> knet_names = { "tcp", "ws", "kcp" };
		ngl::ncjson lnet_json;
		for (const ngl::net_works& lnet : asrv.m_net)
		{
			const auto ltype_idx = static_cast<std::size_t>(lnet.m_type);
			if (ltype_idx >= knet_names.size())
			{
				aparam.clear();
				return false;
			}

			json_net lnet_val;
			lnet_val.m_ip = lnet.m_ip;
			lnet_val.m_nip = lnet.m_nip;
			lnet_val.m_port = lnet.m_port;
			// The network list is encoded as a small JSON object and then URL-escaped.
			ngl::njson::push(lnet_json, { knet_names[ltype_idx] }, lnet_val);
		}

		ngl::ncurl::param(aparam, "net", ngl::tools::url_encode(lnet_json.nonformat_str()));
		return true;
	}
}

using ngl_runtime::detail::apply_robot_cmd;
using ngl_runtime::detail::copy_plan;
using ngl_runtime::detail::init_db_back;
using ngl_runtime::detail::make_log_actor;
using ngl_runtime::detail::make_node_opt;
using ngl_runtime::detail::need_seed_db;
using ngl_runtime::detail::read_line;
using ngl_runtime::detail::robot_plan;
using ngl_runtime::detail::robot_state;
using ngl_runtime::detail::save_seed;
using ngl_runtime::detail::start_node;

startup_error start_robot(int aargc, char** aargv, int* atcp_port);

void init_db_acc(const char* aname, int abeg)
{
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_account ltemp;
		ltemp.set_maccount(std::string(aname) + ngl::tools::lexical_cast<std::string>(li % def_count));
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		ltemp.set_mpassworld("123456");
		ltemp.set_mroleid(ltemp.mid());
		ltemp.set_marea(tab_self_area);

		save_seed<pbdb::ENUM_DB_ACCOUNT>(ltemp);
	}
}

void init_db_acc()
{
	init_db_acc("libo", 1);
	init_db_acc("wac", def_count + 1);
}

// Seed demo role records.
void init_db_role(const char* aname, int abeg)
{
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_role ltemp;

		ngl::i64_actorid lrole_id = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li);
		ltemp.set_mid(lrole_id);
		pbdb::db_brief lrolebase;
		lrolebase.set_mid(ngl::nguid::make(ngl::ACTOR_BRIEF, tab_self_area, li));
		lrolebase.mutable_mbase()->set_mname(std::string(aname) + ngl::tools::lexical_cast<std::string>(li % def_count));
		lrolebase.mutable_mbase()->set_mlv(li);
		lrolebase.mutable_mbase()->set_mmoneygold(0);
		lrolebase.mutable_mbase()->set_mmoneysilver(0);
		lrolebase.mutable_mbase()->set_mcreateutc((int32_t)ngl::localtime::gettime());

		save_seed<pbdb::ENUM_DB_BRIEF>(lrolebase);
		save_seed<pbdb::ENUM_DB_ROLE>(ltemp);
	}
}

void init_db_role()
{
	std::cout << "#########init_db_role()#############" << std::endl;
	std::string lname = std::format("{}_zone{}_", "libo", nconfig.area());
	init_db_role(lname.c_str(), 1);
	lname = std::format("{}_zone{}_", "wac", nconfig.area());
	init_db_role(lname.c_str(), def_count + 1);
}

// Seed demo bag records.
void init_db_bag(const char* aname, int abeg)
{
	(void)aname;
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_bag ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		ltemp.set_mmaxid(1);

		save_seed<pbdb::ENUM_DB_BAG>(ltemp);
	}
}

void init_db_bag()
{
	init_db_bag("libo", 1);
	init_db_bag("wac", def_count + 1);
}

void init_db_task(const char* aname, int abeg)
{
	(void)aname;
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_task ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		auto lrundata = ltemp.mutable_mrundatas();
		std::pair<int32_t, pbdb::db_task::data> lpair;
		lpair.first = 1;
		lpair.second.set_mtaskid(1);
		lpair.second.set_mreceiveutc(0);
		lpair.second.set_mfinshutc(0);
		auto lscheds = lpair.second.mutable_mschedules();
		auto lsch_node = lscheds->Add();
		lsch_node->set_mvalue(1);
		lsch_node->set_msumint(10);
		(*lrundata)[lpair.first] = lpair.second;

		save_seed<pbdb::ENUM_DB_TASK>(ltemp);
	}
}

void init_db_task()
{
	init_db_task("libo", 1);
	init_db_task("wac", def_count + 1);
}

// Seed demo mailbox records.
void init_db_mail(int abeg)
{
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_mail ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));

		save_seed<pbdb::ENUM_DB_MAIL>(ltemp);
	}
}

void init_db_mail()
{
	init_db_mail(1);
	init_db_mail(def_count + 1);
}

void init_db_rkv(int abeg)
{
	for (int li = abeg; li < abeg + def_count; ++li)
	{
		pbdb::db_rolekeyvalue ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		(*ltemp.mutable_mdata())["test1"] = "1";
		(*ltemp.mutable_mdata())["test2"] = "2";
		(*ltemp.mutable_mdata())["test3"] = "3";

		save_seed<pbdb::ENUM_DB_ROLEKEYVALUE>(ltemp);
	}
}

void init_db_rkv()
{
	init_db_rkv(1);
	init_db_rkv(def_count + 1);
}

void init_db_note()
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
	for (int li = 1; li < 10; ++li)
	{
		pbdb::db_notice ltemp;
		ltemp.set_mid((ngl::ttab_servers::instance().const_tab()->m_area * 100) + li);
		if (!ngl::tools::to_utf8(lvec[li], lvec[li]))
		{
			continue;
		}
		ltemp.set_mnotice(lvec[li]);
		const int32_t lnow = static_cast<int32_t>(time(nullptr));
		ltemp.set_mstarttime(lnow);
		ltemp.set_mfinishtime(lnow + 36000);
		save_seed<pbdb::ENUM_DB_NOTICE>(ltemp);
	}
}

void init_db_kv()
{
	pbdb::db_keyvalue ltemp;
	ltemp.set_mid(pbdb::db_keyvalue_ekv_none);
	const int32_t lnow = (int32_t)ngl::localtime::gettime();
	std::string lval = std::format("{}*{}", lnow, ngl::localtime::time2str(lnow, "%y/%m/%d %H:%M:%S"));
	ltemp.set_mvalue(lval);

	save_seed<pbdb::ENUM_DB_KEYVALUE>(ltemp);
}

void init_db_fam()
{
	for (int li = 1; li < 100; ++li)
	{
		pbdb::db_family ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_FAMILY, tab_self_area, li));
		ltemp.set_mcreateutc((int32_t)ngl::localtime::gettime());
		ltemp.set_mexp(100);
		ltemp.set_mlv(1);
		ltemp.set_mname(std::format("FLIBO{}", li));
		ltemp.set_mleader(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		*ltemp.mutable_mmember()->Add() = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li);

		save_seed<pbdb::ENUM_DB_FAMILY>(ltemp);
		
		pbdb::db_familyer ltempfamilyer;
		ltempfamilyer.set_mjoinutc((int32_t)ngl::localtime::gettime());
		ltempfamilyer.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		ltempfamilyer.set_mposition(pbdb::db_familyer_eposition_leader);
		ltempfamilyer.set_mlastsignutc((int32_t)ngl::localtime::gettime());

		save_seed<pbdb::ENUM_DB_FAMILYER>(ltempfamilyer);
	}
}

void init_db_rank()
{
	/*for (int li = 1; li < 100; ++li)
	{
		pbdb::db_ranklist ltemp;
		ltemp.set_mid(ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, li));
		pbdb::rankitem lrankitem;
		lrankitem.set_mtime((int32_t)ngl::localtime::gettime());
		lrankitem.set_mvalue(li);
		(*ltemp.mutable_mitems())[(int)pbdb::eranklist::lv] = lrankitem;

			save_seed<pbdb::ENUM_DB_RANKLIST>(ltemp);
	}*/
}

void init_db_frd()
{
	std::map<ngl::i64_actorid, pbdb::db_friends> lmap;
	for (int li = 0; li < 10; ++li)
	{
		const int lbeg = li * 10;
		const int lend = lbeg + 10;
		for (int lj1 = lbeg; lj1 < lend; ++lj1)
		{
			ngl::i64_actorid lactor1 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, lj1);
			pbdb::db_friends& lfriends = lmap[lactor1];
			lfriends.set_mid(lactor1);
			for (int lj2 = lbeg; lj2 < lend; ++lj2)
			{
				if (lj1 != lj2)
				{
					ngl::i64_actorid lactor2 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, lj2);
					ngl::i64_actorid lactor3 = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, lj2 + 10);
					lfriends.add_mfriends(lactor2);
					lfriends.add_mapplyfriends(lactor3);
				}
			}
		}
	}

	for (auto& apair : lmap)
	{
		save_seed<pbdb::ENUM_DB_FRIENDS>(apair.second);
	}
}

void init_db_tlua()
{
	std::map<ngl::i64_actorid, pbdb::db_testlua> lmap;
	for (int li = 0; li < 10; ++li)
	{
		ngl::i64_actorid lactor1 = ngl::nguid::make(ngl::ACTOR_TESTLUA, tab_self_area, li);
		pbdb::db_testlua& ltestlua = lmap[lactor1];
		ltestlua.set_mid(lactor1);
		for (int lj = 0; lj < 10; ++lj)
		{
			ltestlua.add_mvalue(li * 1000 + lj);
		}
		pbdb::db_testlua::luadata ltemp;
		for (int lidx = 0; lidx < 5; ++lidx)
		{
			ltemp.set_mkey(std::format("key_{}", lidx).c_str());
			ltemp.set_mval(std::format("val_{}", lidx).c_str());
			(*ltestlua.mutable_mdatas())[lidx] = ltemp;
		}
	}

	for (auto& apair : lmap)
	{
		save_seed<pbdb::ENUM_DB_TESTLUA>(apair.second);
	}
}

startup_error start_db(int aargc, char** aargv, int* atcp_port)
{
	return start_node("DB", atcp_port, make_node_opt(), [aargc, aargv]()
		{
			init_db_back(nconfig.db());
			ngl::tdb::tdb_init(false);
			ngl::actor_gmclient::instance();

			if (need_seed_db(aargc, aargv))
			{
				init_db_bag();
				init_db_task();
				init_db_role();
				init_db_note();
				init_db_fam();
				init_db_kv();
				init_db_acc();
				init_db_rkv();
				init_db_rank();
				init_db_frd();
				init_db_tlua();
			}
		});
}

startup_error start_crossdb(int* atcp_port)
{
	return start_node("CROSSDB", atcp_port, make_node_opt(), []()
		{
			init_db_back(nconfig.crossdb());
			ngl::tdb::tcrossdb_init(false);
			ngl::actor_gmclient::instance();
		});
}

startup_error start_world(int* atcp_port)
{
	return start_node("WORLD", atcp_port, make_node_opt(), []()
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

startup_error start_login(int* atcp_port)
{
	return start_node("LOGIN", atcp_port, make_node_opt(), []()
		{
			ngl::actor_login::instance();
			ngl::actor_gmclient::instance();
		});
}

startup_error start_gateway(int* atcp_port)
{
	return start_node("GATEWAY", atcp_port, make_node_opt({ pbnet::KCP_GATEWAY }), []()
		{
			ngl::actor_gateway::instance();
			ngl::actor_gateway_g2c::instance();
			ngl::actor_gateway_c2g::instance();
			ngl::actor_gmclient::instance();
			ngl::actor_kcp::instance();
		});
}

startup_error start_log(int* atcp_port)
{
	return start_node("LOG", atcp_port, make_node_opt({}, true, false, true), []()
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

startup_error start_actor(int* atcp_port)
{
	return start_node("ACTORSERVER", atcp_port, make_node_opt({}, false, false, false), []()
		{
			ngl::actor_server::instance();
			make_log_actor();
			ngl::actor_gmclient::instance();
		});
}

startup_error start_game(int* atcp_port)
{
	return start_node("GAME", atcp_port, make_node_opt({ pbnet::KCP_ROLE }), []()
		{
			ngl::actor_role_manage::instance();
			ngl::actor_create::instance();
			ngl::actor_gmclient::instance();
			ngl::actor_kcp::instance();
		});
}

startup_error start_cross(int* atcp_port)
{
	return start_node("CROSS", atcp_port, make_node_opt(), []()
		{
			ngl::actor_chat::instance();
			ngl::actor_ranklist::instance();
			ngl::actor_gmclient::instance();
		});
}

startup_error start_pushcfg(int* atcp_port)
{
	(void)atcp_port;
	// This process only pushes tab_servers data to the external GM service.
	ngl::xarg_info* linfo = nconfig.info();
	if (linfo == nullptr)
	{
		return startup_error::node_start_failed;
	}

	std::string lgm_url;
	if (!linfo->find("gmurl", lgm_url))
	{
		return startup_error::node_start_failed;
	}

	std::string lapi;
	if (!linfo->find("push_server_config", lapi))
	{
		return startup_error::node_start_failed;
	}

	std::string lurl;
	lurl.reserve(lgm_url.size() + lapi.size() + 1);
	lurl = lgm_url;
	lurl.push_back('/');
	lurl += lapi;

	bool lbad_net = false;

	// Push every configured server entry independently so one bad row does not block logging
	// for the rest of the batch.
	ngl::ttab_servers::instance().foreach_server([&lurl, &lbad_net](ngl::tab_servers* aserver)
		{
			auto lhttp = ngl::ncurl::http();
			ngl::ncurl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
			ngl::ncurl::set_type(lhttp, ngl::ENUM_TYPE_GET);
			ngl::ncurl::set_url(lhttp, lurl);

			std::string lparam;
			if (!ngl_runtime::build_push_cfg(*aserver, lparam))
			{
				lbad_net = true;
				ngl::log_error()->print(
					"[pushserverconfig] invalid network type server:{} type:{}",
					aserver->m_id,
					static_cast<int32_t>(aserver->m_type));
				return;
			}

			ngl::ncurl::set_param(lhttp, lparam);

			ngl::ncurl::set_callback(lhttp, [lsend = std::move(lparam)](int, ngl::http_parm& ahttp)
				{
					ngl::log_error()->print("[{}]->[{}]", lsend, ahttp.m_recvdata);
				});
			ngl::ncurl::send(lhttp);
		});
	return lbad_net ? startup_error::node_start_failed : startup_error::ok;
}

startup_error start_csvserver(int* atcp_port)
{
	return start_node("RELOADCSV", atcp_port, make_node_opt(), []()
		{
			ngl::actor_csvserver::instance();
			ngl::actor_gmclient::instance();
		});
}

startup_error start_robot(int aargc, char** aargv, int* atcp_port)
{
	const ngl_runtime::robot_launch_request lreq = ngl_runtime::build_robot_req(aargc, aargv);
	if (lreq.mode == ngl_runtime::robot_launch_mode::invalid)
	{
		return startup_error::invalid_args;
	}

	std::string lboot_cmd;
	if (lreq.mode != ngl_runtime::robot_launch_mode::interactive)
	{
		lboot_cmd = lreq.command;
		if (lboot_cmd.empty())
		{
			return startup_error::node_start_failed;
		}
	}

	return start_node("ROBOT", atcp_port, make_node_opt(), [lreq, lboot_cmd = std::move(lboot_cmd)]() mutable
		{
			ngl::actor_robot_manage::instance();

			ngl::i32_serverid llogin = ngl::nnodeid::nodeid(static_cast<int16_t>(ngl::ttab_servers::instance().const_tab()->m_login), 1);
			ngl::actor_robot_manage::instance().connect(llogin, ngl::ENET_TCP, [](int)
				{
					std::cout << "connected login server" << std::endl;
				}
			);

			if (lreq.mode == ngl_runtime::robot_launch_mode::interactive)
			{
				// Pure interactive mode behaves like the legacy robot shell.
				while (true)
				{
					std::string lline = read_line();
					if (!lline.empty())
					{
						ngl::actor_robot_manage::parse_command(std::move(lline));
					}
				}
			}

			for (int li = 0; li < 5; ++li)
			{
				ngl::sleep::seconds(1);
				std::cout << "---------------[" << li << "]---------------" << std::endl;
			}

			ngl::actor_robot_manage::parse_command(std::move(lboot_cmd));

			robot_state lstate;
			std::thread([&lstate]()
				{
					// Console input can either mutate the replay plan or execute a normal command immediately.
					while (true)
					{
						std::string lline = read_line();
						if (apply_robot_cmd(lline, lstate))
						{
							continue;
						}
						if (!lline.empty())
						{
							ngl::actor_robot_manage::parse_command(std::move(lline));
						}
					}
				}).detach();

			for (;;)
			{
				if (!lstate.m_enabled.load(std::memory_order_acquire))
				{
					ngl::sleep::seconds(1);
					continue;
				}

				// Snapshot the replay plan so the console thread can keep editing the next iteration.
				const robot_plan lplan = copy_plan(lstate);
				if (lplan.m_interval_ms.empty() || lplan.m_commands.empty())
				{
					ngl::sleep::seconds(1);
					continue;
				}

				for (std::size_t li = 0; li < lplan.m_commands.size() && li < lplan.m_interval_ms.size(); ++li)
				{
					ngl::sleep::milliseconds(lplan.m_interval_ms[li]);
					ngl::actor_robot_manage::parse_command(lplan.m_commands[li]);
				}
			}
		});
}

int ngl_main(int aargc, char** aargv)
{
ngl_startup::start_ctx lctx{};
	const ngl_startup::prep_res lprep = ngl_startup::prep_ctx(aargc, aargv, lctx);
	if (lprep.code != startup_error::ok)
	{
		ngl_startup::log_failure(lprep.code, lctx, lprep.reason);
		return static_cast<int>(lprep.code);
	}

#ifdef WIN32
	SetConsoleTitle(nconfig.servername().c_str());
#endif

	startup_error lrc = startup_error::ok;
	// Each node type shares the same bootstrap path but installs a different actor set.
	switch (nconfig.nodetype())
	{
	case ngl::DB:
		lrc = start_db(aargc, aargv, &lctx.tcp_port);
		break;
	case ngl::GAME:
		lrc = start_game(&lctx.tcp_port);
		break;
	case ngl::ACTORSERVER:
		lrc = start_actor(&lctx.tcp_port);
		break;
	case ngl::LOG:
		lrc = start_log(&lctx.tcp_port);
		break;
	case ngl::GATEWAY:
		lrc = start_gateway(&lctx.tcp_port);
		break;
	case ngl::LOGIN:
		lrc = start_login(&lctx.tcp_port);
		break;
	case ngl::WORLD:
		lrc = start_world(&lctx.tcp_port);
		break;
	case ngl::RELOADCSV:
		lrc = start_csvserver(&lctx.tcp_port);
		break;
	case ngl::ROBOT:
		lrc = start_robot(aargc, aargv, &lctx.tcp_port);
		break;
	case ngl::CROSS:
		lrc = start_cross(&lctx.tcp_port);
		break;
	case ngl::CROSSDB:
		lrc = start_crossdb(&lctx.tcp_port);
		break;
	case ngl::PUSHSERVERCONFIG:
		lrc = start_pushcfg(&lctx.tcp_port);
		break;
	default:
		lrc = startup_error::invalid_node_type;
		break;
	}

	if (lrc != startup_error::ok)
	{
		ngl_startup::log_failure(lrc, lctx, "node start failed");
		return static_cast<int>(lrc);
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
