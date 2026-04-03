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

#include "tools/tools/tools_time_wheel.h"
#include "tools/tools/tools_thread.h"
#include "tools/serialize/nrate.h"
#include "tools/serialize/pack.h"
#include "tools/log/nlog.h"
#include "net/asio_base.h"
#include "net/node_pack.h"
#include "tools/type.h"
#include "ikcp.h"

#include <memory>
#include <map>

namespace ngl
{	
	class asio_kcp;

	extern tools::time_wheel m_kcptimer;

	struct kcp_endpoint :public std::enable_shared_from_this<kcp_endpoint>
	{
		basio::ip::udp::endpoint	m_endpoint;			// Remote UDP endpoint.
		i32_sessionid				m_session = 0;		// Local synthetic session id.
		std::string					m_ip;				// Cached remote IP string.
		i16_port					m_port = 0;			// Cached remote port.
		asio_kcp*					m_asiokcp = nullptr; // Owning asio_kcp transport.
		ikcpcb*						m_kcp = nullptr;	// Underlying KCP control block.
		bool						m_isconnect = false; // True after the logical KCP handshake completes.
		i64_actorid					m_client = 0;		// Logical client actor id.
		i64_actorid					m_server = 0;		// Logical server actor id.
		int64_t						m_timerid = 0;		// Periodic ikcp_update timer id.
		npack_list					m_npacklist;

		typedef int (*output)(const char* buf, int len, struct IKCPCB* kcp, void* user);

		// Create/destroy the KCP control block and expose thin wrappers over ikcp_* APIs.
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
