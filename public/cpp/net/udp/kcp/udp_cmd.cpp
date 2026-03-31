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
// File overview: Implements logic for kcp.


#include "net/udp/kcp/asio_kcp.h"
#include "net/udp/kcp/udp_cmd.h"

namespace ngl
{
	bool udp_cmd::run_cmd(asio_kcp* ap, std::shared_ptr<kcp_endpoint>& apstruct, const char* abuf, int32_t alen)
	{
		if (alen < ecmd_minlen)
		{
			return false;
		}
		if (memcmp(abuf, "ecmd*", ecmd_minlen) != 0)
		{
			return false;
		}
		try
		{
			// Format: "ecmd*<number>*<json>".
			std::string lecmd;
			int32_t lnum = 0;
			std::string ljson;
			if (tools::splite(abuf, "*", lecmd, lnum, ljson) == false)
			{
				return false;
			}
			handle_cmd::function(static_cast<ecmd>(lnum), ap, apstruct, ljson);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void udp_cmd::register_fun(ecmd anum, const ecmd_callback& afun)
	{
		handle_cmd::add(anum) = afun;
	}

	bool udp_cmd::sendcmd(asio_kcp* akcp, i32_sessionid asession, udp_cmd::ecmd acmd, const char* ajson)
	{
		std::string lbuff = std::format("ecmd*{}*{}", (int)acmd, ajson);
		if (lbuff.empty())
		{
			log_error()->print("udp_cmd::sendcmd fail [{}][{}]", (int)acmd, ajson);
			return false;
		}
		akcp->send(asession, lbuff.c_str(), (int)lbuff.size());
		return true;
	}
}//namespace ngl
