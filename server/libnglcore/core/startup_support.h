// File overview: Declares interfaces for core.

#pragma once

#include "server_main.h"

#include <cstdint>
#include <string>

namespace ngl_startup
{
	struct context
	{
		std::string node_name;
		int32_t area = 0;
		int32_t tcount = 0;
		std::string config_file;
		int tcp_port = -1;
		int node_type = -1;
	};

	struct prepare_result
	{
		startup_error code = startup_error::ok;
		const char* reason = "";
	};

	void log_failure(startup_error code, const context& ctx, const char* reason);
	prepare_result prepare_context(int argc, char** argv, context& ctx);
}