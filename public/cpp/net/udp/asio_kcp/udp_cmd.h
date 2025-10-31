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
#pragma once

#include "session_manage.h"
#include "cmd.h"

namespace ngl
{
	class udp_cmd
	{
	public:
		enum ecmd
		{
			ecmd_connect,				// 发起连接
			ecmd_connect_ret,			// 被发起连接者的返回
			ecmd_ping,					// 定时ping
			ecmd_close,					// 主动断开连接
			ecmd_close_ret,             // 主动断开连接的返回
			ecmd_minlen = sizeof("ecmd*") - 1,
		};
		using ecmd_callback = std::function<void(asio_kcp*, ptr_se&, const std::string&)>;

		using handle_cmd = cmd<udp_cmd, ecmd, asio_kcp*, ptr_se&, const std::string&>;

		static bool run_cmd(asio_kcp* ap, ptr_se& apstruct, const char* abuf, int32_t alen);

		static void register_fun(ecmd anum, const ecmd_callback& afun);

		static bool sendcmd(asio_kcp* akcp, i32_sessionid asession, ecmd acmd, const std::string& ajson);
	};

}//namespace ngl