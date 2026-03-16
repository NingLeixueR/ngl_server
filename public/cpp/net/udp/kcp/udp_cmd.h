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
// File overview: Declares interfaces for kcp.

#pragma once

#include "net/udp/kcp/kcp_session.h"
#include "tools/cmd.h"

namespace ngl
{
	class udp_cmd
	{
	public:
		enum ecmd
		{
			ecmd_connect,				// Connection
			ecmd_connect_ret,			// Connection return
			ecmd_ping,					// Ping
			ecmd_close,					// Connection
			ecmd_close_ret,             // Connection return
			ecmd_minlen = sizeof("ecmd*") - 1,
		};
		using ecmd_callback = std::function<void(asio_kcp*, ptr_se&, const std::string&)>;

		using handle_cmd = cmd<udp_cmd, ecmd, asio_kcp*, ptr_se&, const std::string&>;

		static bool run_cmd(asio_kcp* ap, ptr_se& apstruct, const char* abuf, int32_t alen);

		static void register_fun(ecmd anum, const ecmd_callback& afun);

		static bool sendcmd(asio_kcp* akcp, i32_sessionid asession, ecmd acmd, const char* ajson);
	};
}//namespace ngl