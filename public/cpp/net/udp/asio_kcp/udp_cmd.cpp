/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "udp_cmd.h"
#include "asio_kcp.h"

namespace ngl
{
	bool udp_cmd::run_cmd(asio_kcp* ap, ptr_se& apstruct, const char* abuf, int32_t alen)
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
			std::string lecmd;
			int32_t lnum = 0;
			std::string ljson;
			if (tools::splite(abuf, "*", lecmd, lnum, ljson) == false)
			{
				return false;
			}
			handle_cmd::function((ecmd)lnum, ap, apstruct, ljson);
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

	bool udp_cmd::sendcmd(asio_kcp* akcp, i32_sessionid asession, udp_cmd::ecmd acmd, const std::string& ajson)
	{
		std::string lbuff = std::format("ecmd*{}*{}", (int)acmd, ajson.c_str());
		if (lbuff.empty())
		{
			log_error()->print("udp_cmd::sendcmd fail [{}][{}]", (int)acmd, ajson);
			return false;
		}
		akcp->send(asession, lbuff.c_str(), (int)lbuff.size());
		return true;
	}

}//namespace ngl