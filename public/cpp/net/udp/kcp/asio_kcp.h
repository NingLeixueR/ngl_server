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

#include "net/udp/kcp/kcp_endpoint.h"
#include "net/udp/kcp/kcp_session.h"
#include "tools/serialize/pack.h"
#include "net/udp/kcp/udp_cmd.h"
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
	class asio_kcp
	{
		asio_kcp() = delete;
		asio_kcp(const asio_kcp&) = delete;
		asio_kcp& operator=(const asio_kcp&) = delete;

	public:
		enum
		{
			e_buff_byte = 1500,
			e_buffrecv_byte = 10240,
			e_close_intervalms = 1* localtime::MILLISECOND,
			e_waitrecv_intervalms = 1* localtime::MILLISECOND,
		};
	private:
		kcp_session							m_session;
		std::function<void(i32_session)>	m_connectfun = nullptr;
		bpool								m_pool;
		basio::io_context					m_context;
		basio_ioservicework					m_work_guard;
		asio_udp::socket					m_socket;
		asio_udp_endpoint					m_remoteport;
		char								m_buff[e_buff_byte] = { 0 };
		std::size_t							m_bytes_received = 0;
		char								m_buffrecv[e_buffrecv_byte] = { 0 };
		std::mutex							m_waitmutex;
		std::function<void(char*, int)>		m_wait = nullptr;
		asio_udp_endpoint					m_waitendpoint;
		i16_port							m_port = 0;
		std::jthread						m_thread;
		nrate								m_rate;
	public:
		explicit asio_kcp(i16_port port);

		~asio_kcp();
	private:
		bool async_send_copy(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(const basio_errorcode&)>& aerrorfun = {});

		void func_ecmd_connect()const;

		void func_ecmd_connect_ret()const;

		void func_ecmd_close()const;
	public:
		// # Send udppack
		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		// # Send udppackand return
		bool sendu_waitrecv(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun);

		// # Send a pack through KCP
		bool send_server(i32_sessionid asessionid, const std::shared_ptr<pack>& apack);

		// # Throughkcptoallclientsendpack
		bool send_server(const std::shared_ptr<pack>& apack);

		// # Throughkcptospecifiedareaallclientsendpack
		bool sendpackbyarea(i16_area aarea, const std::shared_ptr<pack>& apack);

		// # Send a pack through KCP
		bool send_server(const asio_udp_endpoint& aendpoint, const std::shared_ptr<pack>& apack);

		// # Send a pack through KCP
		bool sendpackbyactorid(i64_actorid aactorid, const std::shared_ptr<pack>& apack);

		// # Connection
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aserver
			, i64_actorid aclient
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		);

		// # Connection
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aserver
			, i64_actorid aclient
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		);

		// # Findconnection actor
		i64_actorid find_server(i32_session asession);
		i64_actorid find_client(i32_session asession);
		bool find_actorid(i32_session asession, i64_actorid& aserver, i64_actorid& aclient);

		// # Actoridgetsession
		i32_session find_session(i64_actorid aclient);

		// # Closeconnection
		void close(i32_session asession);

		void close_net(i32_session asession);

		// # Connection
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient);

		bool sempack(const ptr_se& apstruct, const char* abuff, int abufflen);

		void start();

		bool send(i32_sessionid asessionid, const char* buf, int len);

		int send(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		int sendbuff(i32_session asession, const char* buf, int len);

		int sendbuff(const asio_udp_endpoint& aendpoint, const char* buf, int len);
	};
}// namespace ngl