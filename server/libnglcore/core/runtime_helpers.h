// File overview: Declares interfaces for core.

#pragma once

#include "actor/tab/ttab_servers.h"

#include <string>

namespace ngl_runtime
{
	enum class robot_launch_mode
	{
		invalid,
		interactive, // Read commands from stdin.
		login,       // Auto-run a single "login <account>" command.
		logins,      // Auto-run a batched "logins <prefix> <count> <start>" command.
	};

	struct robot_launch_request
	{
		// Parsed robot bootstrap mode plus the synthesized command line, if any.
		robot_launch_mode mode = robot_launch_mode::invalid;
		std::string command;
	};

	// Convert raw CLI arguments into a higher-level robot launch request.
	robot_launch_request build_robot_req(int aargc, char** aargv);

	// Serialize a tab_servers row into the HTTP query string expected by the GM endpoint.
	bool build_push_cfg(const ngl::tab_servers& aserver, std::string& aparam);
}
