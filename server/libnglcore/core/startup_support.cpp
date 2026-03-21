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
	void apply_srv(const ngl_startup::start_ctx& aCTX, const ngl::tab_servers& aTAB)
	{
		// Compose the runtime node id and a readable process name from tab_servers metadata.
		nconfig.set_nodeid(aTAB.m_id, aCTX.tcount);
		if (aCTX.area < 0)
		{
			nconfig.set_servername(std::format("node_{}__{}_{}", aCTX.node_name, -aCTX.area, aCTX.tcount));
		}
		else
		{
			nconfig.set_servername(std::format("node_{}_{}_{}", aCTX.node_name, aTAB.m_area, aCTX.tcount));
		}
	}
}

namespace ngl_startup
{
	void log_failure(startup_error code, const start_ctx& ctx, const char* reason)
	{
		ngl::log_error()->print(
			"[startup][code:{}] reason:{} node:{} type:{} area:{} tcount:{} config:{} port:{}"
			, static_cast<int>(code)
			, reason
			, ctx.node_name
			, ctx.node_type
			, ctx.area
			, ctx.tcount
			, ctx.config_file
			, ctx.tcp_port
		);
	}

	prep_res prep_ctx(int argc, char** argv, start_ctx& ctx)
	{
		ctx = start_ctx{};

		// Expected CLI shape: <program> <node_name> <area> <tcount> [extra args...].
		if (argc < 4 || argv == nullptr)
		{
			return { startup_error::invalid_args, "argc < 4" };
		}

		// Parse the minimal identity first so later failures can be logged with context.
		ctx.node_name = argv[1] == nullptr ? "" : argv[1];
		if (!ngl::tools::try_lexical_cast(argv[2], ctx.area) || !ngl::tools::try_lexical_cast(argv[3], ctx.tcount))
		{
			return { startup_error::invalid_args, "area/tcount parse failed" };
		}

		// Resolve node type from the symbolic server name before loading config files.
		nconfig.init();
		if (!nconfig.set_server(ctx.node_name.c_str()))
		{
			ctx.node_type = static_cast<int>(nconfig.nodetype());
			return { startup_error::invalid_node_type, "invalid node type" };
		}
		ctx.node_type = static_cast<int>(nconfig.nodetype());

		// Each process reads its own config variant, for example "game_1".
		const std::string lCFG = std::format("{}_{}", ctx.node_name, ctx.tcount);
		if (!nconfig.load("./config", lCFG))
		{
			ctx.config_file = nconfig.config_file();
			const startup_error code = ngl::tools::file_exists(ctx.config_file) ?
				startup_error::config_load_failed : startup_error::config_not_found;
			return { code, "config load failed" };
		}
		ctx.config_file = nconfig.config_file();

		// CSV tables are namespaced by node name so test and tool processes can load different views.
		ngl::csv_base::set_path("./csv", ctx.node_name);

		// tab_servers is the authoritative source for network layout and thread counts.
		const ngl::tab_servers* lTAB = ngl::ttab_servers::instance().const_tab(ctx.node_name, ctx.area);
		if (lTAB == nullptr)
		{
			return { startup_error::tab_server_missing, "tab_servers missing" };
		}

		detail::apply_srv(ctx, *lTAB);
		return {};
	}
}
