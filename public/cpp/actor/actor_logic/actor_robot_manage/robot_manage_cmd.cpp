/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements small parsing helpers for robot-manage commands.

#include "tools/arg_options.h"

#include <string>
#include <string_view>
#include <vector>

namespace ngl::robot_manage_cmd
{
	std::vector<std::string> split_cmd(std::string_view aargs)
	{
		return arg_options::split_command_line(aargs);
	}

	bool strip_help(std::vector<std::string>& aargs)
	{
		bool lhelp_req = false;
		bool lend_opt = false;
		std::vector<std::string> largs;
		largs.reserve(aargs.size());
		for (const std::string& larg : aargs)
		{
			if (!lend_opt && larg == "--")
			{
				lend_opt = true;
				largs.push_back(larg);
				continue;
			}

			if (!lend_opt && (larg == "-h" || larg == "--help"))
			{
				lhelp_req = true;
				continue;
			}

			largs.push_back(larg);
		}
		aargs.swap(largs);
		return lhelp_req;
	}
}
