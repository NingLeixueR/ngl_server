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