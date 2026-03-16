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

#include "tools/serialize/nrate.h"
#include "tools/serialize/pack.h"
#include "tools/threadtools.h"
#include "tools/time_wheel.h"
#include "tools/log/nlog.h"
#include "net/asio_base.h"
#include "tools/type.h"
#include "ikcp.h"

#include <memory>
#include <map>

namespace ngl
{
	using asio_udp = basio::ip::udp;
	using asio_udp_endpoint = asio_udp::endpoint;

	class asio_kcp;

	extern time_wheel m_kcptimer;

	struct kcp_endpoint
	{
		asio_udp_endpoint	m_endpoint;
		i32_sessionid		m_session = 0;
		std::string			m_ip;
		i16_port			m_port = 0;
		asio_kcp*			m_asiokcp = nullptr;
		ikcpcb*				m_kcp = nullptr;
		bool				m_isconnect = false;		// Whether tokcp_cmd::ecmd_connect or ecmd_connect_ret
		i64_actorid			m_client = 0;		// robot
		i64_actorid			m_server = 0;		// server
		int64_t				m_timerid = 0;

		typedef int (*output)(const char* buf, int len, struct IKCPCB* kcp, void* user);

		~kcp_endpoint();

		static std::string ip(kcp_endpoint* ap);

		static i16_port port(kcp_endpoint* ap);

		void create(int32_t aconv, uint32_t asessionid, void* auser);

		void removetimer();

		int setmtu(int mtu);

		void setoutput(output afun);

		int nodelay(int nodelay, int interval, int resend, int nc);

		int wndsize(int sndwnd, int rcvwnd);

		void update(uint32_t current);

		int input(const char* data, long size);

		int recv(char* buffer, int len);

		int send(const char* buffer, int len);

		void flush();

		void release();
	};
}//namespace ngl