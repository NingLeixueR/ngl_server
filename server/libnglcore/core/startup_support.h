// File overview: Declares interfaces for core.

#pragma once

#include "server_main.h"

#include <cstdint>
#include <string>

namespace ngl_startup
{
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
}
