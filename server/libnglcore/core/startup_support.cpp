// File overview: Implements logic for core.

#include "startup_support.h"

#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_servers.h"
#include "tools/tab/csv/csv.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools.h"

#include <format>

namespace ngl_startup::detail
{
	void apply_srv(const ngl_startup::start_ctx& actx, const ngl::tab_servers& atab)
	{
		// Compose the runtime node id and a readable process name from tab_servers metadata.
		nconfig.set_nodeid(atab.m_id, actx.tcount);
		if (actx.area < 0)
		{
			nconfig.set_servername(std::format("node_{}__{}_{}", actx.node_name, -actx.area, actx.tcount));
		}
		else
		{
			nconfig.set_servername(std::format("node_{}_{}_{}", actx.node_name, atab.m_area, actx.tcount));
		}
	}
}

namespace ngl_startup
{
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
}
