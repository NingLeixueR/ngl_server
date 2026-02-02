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

#include "session_endpoint.h"
#include "threadtools.h"
#include "time_wheel.h"
#include "asio_base.h"
#include "type.h"
#include "pack.h"
#include "nlog.h"
#include "ikcp.h"

#include <asio.hpp>
#include <memory>
#include <map>

namespace ngl
{
	using ptr_se = std::shared_ptr<session_endpoint>;

	class kcp_session
	{
		kcp_session() = delete;
		kcp_session(const kcp_session&) = delete;
		kcp_session& operator=(const kcp_session&) = delete;

		std::map<i32_sessionid, ptr_se>						m_dataofsession;
		std::map<i64_actorid, i32_sessionid>				m_actoridofsession;		// client/server对应的sessionid

		std::map<std::string, std::map<i16_port, ptr_se>>	m_dataofendpoint;
		int32_t												m_sessionid;
		std::shared_mutex									m_mutex;
		asio_kcp*											m_asiokcp;

		ptr_se find_info(i32_sessionid asession);
		ptr_se find_info(const asio_udp_endpoint& aendpoint);
	public:
		enum
		{
			ekcp_update_intervalms = 10,		// 调用kcp api update的间隔(单位毫秒)
		};

		kcp_session(asio_kcp* asiokcp);

		ptr_se add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactoridlocal, i64_actorid aactoridremote);

		ptr_se reset_add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactoridlocal, i64_actorid aactoridremote);

		void erase(const asio_udp_endpoint& aendpoint);

		void erase(i32_sessionid asession);

		ptr_se find(i32_sessionid asession);

		ptr_se findbyactorid(i64_actorid aactorid);

		ptr_se find(const asio_udp_endpoint& aendpoint);

		asio_udp_endpoint* find_endpoint(i32_sessionid asession);

		void foreach(const std::function<void(ptr_se&)>& acall);

		void foreachbyarea(i16_area aarea, const std::function<void(ptr_se&)>& acall);
	};
}//namespace ngl