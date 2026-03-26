// File overview: Implements logic for core.

#include "tools/tab/xml/xml_serialize.h"
#include "tools/arg_options.h"
#include "start_csvserver.h"
#include "start_crossdb.h"
#include "start_pushcfg.h"
#include "start_gateway.h"
#include "start_robot.h"
#include "start_cross.h"
#include "start_login.h"
#include "start_world.h"
#include "start_actor.h"
#include "server_main.h"
#include "init_server.h"
#include "start_node.h"
#include "start_game.h"
#include "start_log.h"
#include "start_db.h"

#include <string_view>
#include <type_traits>
#include <atomic>
#include <cctype>
#include <ctime>
#include <mutex>

struct start_ctx
{
	// Parsed runtime identity for the current process.
	std::string node_name;
	int32_t area = 0;
	int32_t tcount = 0;
	std::string config_file;
	int tcp_port = -1;
	int node_type = -1;
};

struct prep_res
{
	// Keep both a machine-friendly code and a short log-friendly reason.
	startup_error code = startup_error::ok;
	const char* reason = "";
};

// Emit a uniform startup failure log that includes the resolved context.
void log_failure(startup_error acode, const start_ctx& actx, const char* areason);

// Parse CLI arguments, load config/csv metadata, and resolve the target server row.
prep_res prep_ctx(int aargc, char** aargv, start_ctx& actx);

namespace detail
{
	void apply_srv(start_ctx& actx, const ngl::tab_servers& asrv)
	{
		actx.node_type = static_cast<int>(asrv.m_type);
		nconfig.set_nodeid(static_cast<int16_t>(asrv.m_id), static_cast<int16_t>(actx.tcount));
		if (actx.area < 0)
		{
			nconfig.set_servername(std::format("node_{}__{}_{}", actx.node_name, -actx.area, actx.tcount));
		}
		else
		{
			nconfig.set_servername(std::format("node_{}_{}_{}", actx.node_name, asrv.m_area, actx.tcount));
		}

		ngl::net_works lnet;
		if (ngl::ttab_servers::instance().get_nworks(&asrv, ngl::ENET_PROTOCOL::ENET_TCP, actx.tcount, lnet))
		{
			actx.tcp_port = lnet.m_port;
		}
	}
}

prep_res prep_ctx(int aargc, char** aargv, start_ctx& actx)
{
	actx = start_ctx{};

	// Expected CLI shape: <program> <node_name> <area> <tcount> [extra args...].
	if (aargc < 4 || aargv == nullptr)
	{
		return { startup_error::invalid_args, "argc < 4" };
	}

	// Parse the minimal identity first so later failures can be logged with context.
	actx.node_name = aargv[1] == nullptr ? "" : aargv[1];
	if (!ngl::tools::try_lexical_cast(aargv[2], actx.area) || !ngl::tools::try_lexical_cast(aargv[3], actx.tcount))
	{
		return { startup_error::invalid_args, "area/tcount parse failed" };
	}

	// Resolve node type from the symbolic server name before loading config files.
	nconfig.init();
	if (!nconfig.set_server(actx.node_name.c_str()))
	{
		actx.node_type = static_cast<int>(nconfig.nodetype());
		return { startup_error::invalid_node_type, "invalid node type" };
	}
	actx.node_type = static_cast<int>(nconfig.nodetype());

	// Each process reads its own config variant, for example "game_1".
	const std::string lcfg = std::format("{}_{}", actx.node_name, actx.tcount);
	if (!nconfig.load("./config", lcfg))
	{
		actx.config_file = nconfig.config_file();
		const startup_error lcode = ngl::tools::file_exists(actx.config_file) ?
			startup_error::config_load_failed : startup_error::config_not_found;
		return { lcode, "config load failed" };
	}
	actx.config_file = nconfig.config_file();

	// CSV tables are namespaced by node name so test and tool processes can load different views.
	ngl::csv_base::set_path("./csv", actx.node_name);

	// tab_servers is the authoritative source for network layout and thread counts.
	const ngl::tab_servers* ltab = ngl::ttab_servers::instance().const_tab(actx.node_name, actx.area);
	if (ltab == nullptr)
	{
		return { startup_error::tab_server_missing, "tab_servers missing" };
	}

	detail::apply_srv(actx, *ltab);
	return {};
}

void log_failure(startup_error acode, const start_ctx& actx, const char* areason)
{
	ngl::log_error()->print(
		"[startup][code:{}] reason:{} node:{} type:{} area:{} tcount:{} config:{} port:{}"
		, static_cast<int>(acode)
		, areason
		, actx.node_name
		, actx.node_type
		, actx.area
		, actx.tcount
		, actx.config_file
		, actx.tcp_port
	);
}

int ngl_main(int aargc, char** aargv)
{
	start_ctx lctx{};
	const prep_res lprep = prep_ctx(aargc, aargv, lctx);
	if (lprep.code != startup_error::ok)
	{
		log_failure(lprep.code, lctx, lprep.reason);
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
