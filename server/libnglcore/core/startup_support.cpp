// File overview: Implements logic for core.

#include "startup_support.h"

#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_servers.h"
#include "tools/tab/csv/csv.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools.h"

#include <format>

namespace
{
	bool parse_startup_int(const char* text, int32_t& value)
	{
		if (text == nullptr)
		{
			return false;
		}

		try
		{
			value = ngl::tools::lexical_cast<int32_t>(text);
			return true;
		}
		catch (const boost::bad_lexical_cast&)
		{
			return false;
		}
		catch (const std::string&)
		{
			return false;
		}
	}

	void apply_server_identity(const ngl_startup::context& ctx, const ngl::tab_servers& tab)
	{
		// Compose the runtime node id and a readable process name from tab_servers metadata.
		nconfig.set_nodeid(tab.m_id, ctx.tcount);
		if (ctx.area < 0)
		{
			nconfig.set_servername(std::string(std::format("node_{}__{}_{}", ctx.node_name, -ctx.area, ctx.tcount)));
		}
		else
		{
			nconfig.set_servername(std::string(std::format("node_{}_{}_{}", ctx.node_name, tab.m_area, ctx.tcount)));
		}
	}
}

namespace ngl_startup
{
	void log_failure(startup_error code, const context& ctx, const char* reason)
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

	prepare_result prepare_context(int argc, char** argv, context& ctx)
	{
		ctx = context{};

		// Expected CLI shape: <program> <node_name> <area> <tcount> [extra args...].
		if (argc < 4 || argv == nullptr)
		{
			return { startup_error::invalid_args, "argc < 4" };
		}

		// Parse the minimal identity first so later failures can be logged with context.
		ctx.node_name = argv[1] == nullptr ? "" : argv[1];
		if (!parse_startup_int(argv[2], ctx.area) || !parse_startup_int(argv[3], ctx.tcount))
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
		const std::string config_name = std::format("{}_{}", ctx.node_name, ctx.tcount);
		if (!nconfig.load("./config", config_name))
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
		const ngl::tab_servers* tab = ngl::ttab_servers::instance().const_tab(ctx.node_name, ctx.area);
		if (tab == nullptr)
		{
			return { startup_error::tab_server_missing, "tab_servers missing" };
		}

		apply_server_identity(ctx, *tab);
		return {};
	}
}
