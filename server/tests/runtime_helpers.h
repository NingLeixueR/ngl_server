// File overview: Declares small runtime helper utilities used by tests.

#pragma once

#include "actor/tab/csvtable.h"

#include <format>
#include <string>
#include <string_view>

bool build_push_cfg(const ngl::tab_servers& asrv, std::string& aparam);

namespace ngl_runtime
{
	enum class robot_launch_mode
	{
		invalid = 0,
		interactive,
		login,
		logins,
	};

	struct robot_req
	{
		robot_launch_mode mode = robot_launch_mode::invalid;
		std::string command;
	};

	namespace detail
	{
		inline std::string_view get_arg(int aidx, char** aargv)
		{
			if (aargv == nullptr || aidx < 0 || aargv[aidx] == nullptr)
			{
				return {};
			}
			return aargv[aidx];
		}

		inline bool valid_proto(std::string_view aproto)
		{
			return aproto.empty() || aproto == "tcp" || aproto == "ws" || aproto == "wss";
		}
	}

	inline robot_req build_robot_req(int aargc, char** aargv)
	{
		robot_req lreq;
		if (aargc == 4)
		{
			lreq.mode = robot_launch_mode::interactive;
			return lreq;
		}

		if (aargc == 5 || aargc == 6)
		{
			const std::string_view lrobot = detail::get_arg(4, aargv);
			const std::string_view lproto = aargc == 6 ? detail::get_arg(5, aargv) : std::string_view{};
			if (lrobot.empty() || !detail::valid_proto(lproto))
			{
				return lreq;
			}

			lreq.mode = robot_launch_mode::login;
			lreq.command = lproto.empty() ?
				std::format("login {}", lrobot) :
				std::format("login {} {}", lrobot, lproto);
			return lreq;
		}

		if (aargc == 7 || aargc == 8)
		{
			const std::string_view lname = detail::get_arg(4, aargv);
			const std::string_view lbeg = detail::get_arg(5, aargv);
			const std::string_view lend = detail::get_arg(6, aargv);
			const std::string_view lproto = aargc == 8 ? detail::get_arg(7, aargv) : std::string_view{};
			if (lname.empty() || lbeg.empty() || lend.empty() || !detail::valid_proto(lproto))
			{
				return lreq;
			}

			lreq.mode = robot_launch_mode::logins;
			lreq.command = lproto.empty() ?
				std::format("logins {} {} {}", lname, lbeg, lend) :
				std::format("logins {} {} {} {}", lname, lbeg, lend, lproto);
			return lreq;
		}

		return lreq;
	}

	inline bool build_push_cfg(const ngl::tab_servers& asrv, std::string& aparam)
	{
		return ::build_push_cfg(asrv, aparam);
	}
}
