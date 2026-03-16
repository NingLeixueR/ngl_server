// File overview: Declares interfaces for core.

#pragma once

#include "actor/tab/ttab_servers.h"

#include <string>
#include <vector>

namespace ngl_runtime
{
	enum class robot_launch_mode
	{
		invalid,
		interactive,
		login,
		logins,
	};

	struct robot_launch_request
	{
		robot_launch_mode mode = robot_launch_mode::invalid;
		std::string command;
	};

	robot_launch_request build_robot_launch_request(int argc, char** argv);
	std::vector<std::string> split_command_line(std::string line);
	bool build_push_server_config_param(const ngl::tab_servers& server, std::string& param);
}