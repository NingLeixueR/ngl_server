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
#include "tools/tools/tools_thread.h"
#include "tools/tools/tools_time_wheel.h"
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
			e_close_intervalms = 1* tools::time::MILLISECOND,
			e_waitrecv_intervalms = 1* tools::time::MILLISECOND,
		};
	private:
		kcp_session							m_session;			// Session table keyed by KCP session id / actor id / endpoint.
		std::function<void(i32_session)>	m_connectfun = nullptr; // Pending outbound connect callback.
		bpool								m_pool;				// Temporary pack allocation pool.
		basio::io_context					m_context;			// Dedicated UDP io_context.
		basio_ioservicework					m_work_guard;		// Keeps m_context alive until shutdown.
		basio::ip::udp::socket					m_socket;			// Shared UDP socket bound to m_port.
		basio::ip::udp::endpoint					m_remoteport;		// Source endpoint of the last received datagram.
		char								m_buff[e_buff_byte] = { 0 }; // Raw UDP receive buffer.
		char								m_buffrecv[e_buffrecv_byte] = { 0 }; // Buffer for packets popped out of KCP.
		std::mutex							m_waitmutex;		// Protects synchronous wait-recv helper state.
		std::function<void(char*, int)>		m_wait = nullptr;	// Optional one-shot raw UDP reply callback.
		basio::ip::udp::endpoint					m_waitendpoint;		// Endpoint expected by m_wait.
		i16_port							m_port = 0;			// Bound UDP port.
		std::jthread						m_thread;			// Background thread running m_context.
		nrate								m_rate;				// Per-session traffic limiter.
	public:
		explicit asio_kcp(i16_port port);

		~asio_kcp() noexcept;
	private:
		template <typename TSEQ>
		bool send(const std::shared_ptr<kcp_endpoint>& akcp
			, const std::shared_ptr<pack>& apack
			, const TSEQ& adata
		)
		{
			m_socket.async_send_to(adata, akcp->m_endpoint,
				[this, apack, akcp](const basio_errorcode& ec, std::size_t)
				{
					if (ec)
					{
						log_error()->print("asio_tcp::handle_write[{}]", ec.message().c_str());
						close(akcp->m_session);
						return;
					}
					do_send(akcp);
				}
			);
			return true;
		}
		// Register built-in control commands used during KCP handshake/close.
		bool async_send(const std::shared_ptr<kcp_endpoint>& akcp, const std::shared_ptr<pack>& apack);

		void do_send(const std::shared_ptr<kcp_endpoint>& akcp);

		void func_ecmd_connect()const;

		void func_ecmd_connect_ret()const;

		void func_ecmd_close()const;
	public:
		// Send raw UDP datagrams, KCP packs, or KCP-routed actor traffic.
		bool sendu(const basio::ip::udp::endpoint& aendpoint, const char* buf, int len);

		bool sendu_waitrecv(const basio::ip::udp::endpoint& aendpoint, const std::shared_ptr<pack>& apack);
		// Send one raw UDP datagram and wait for the first reply from the same endpoint.
		bool sendu_waitrecv(const basio::ip::udp::endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun);

		// Route one serialized pack through an existing KCP session id.
		bool send_server(i32_sessionid asessionid, std::shared_ptr<pack>& apack);

		// Broadcast one serialized pack to every active KCP session.
		bool send_server(std::shared_ptr<pack>& apack);

		// Broadcast one serialized pack only to sessions in the specified area.
		bool sendpackbyarea(i16_area aarea, std::shared_ptr<pack>& apack);

		// Route one serialized pack by looking up the KCP session for a remote endpoint.
		//bool send_server(const basio::ip::udp::endpoint& aendpoint, const std::shared_ptr<pack>& apack);

		// Route one serialized pack to the session currently owned by `aactorid`.
		bool sendpackbyactorid(i64_actorid aactorid, std::shared_ptr<pack>& apack);

		bool send(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool send(i32_sessionid asessionid, const char* buf, int32_t len);

		// Start an outbound KCP handshake toward a remote UDP endpoint.
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aserver
			, i64_actorid aclient
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		);

		// Start an outbound KCP handshake toward a prebuilt UDP endpoint.
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aserver
			, i64_actorid aclient
			, const basio::ip::udp::endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		);

		// Resolve actor/session relationships managed by kcp_session.
		i64_actorid find_server(i32_session asession);
		i64_actorid find_client(i32_session asession);
		bool find_actorid(i32_session asession, i64_actorid& aserver, i64_actorid& aclient);

		// Resolve an actor id back to its current KCP session id.
		i32_session find_session(i64_actorid aclient);

		// Remove a KCP session from the session table.
		void close(i32_session asession);

		void close_net(i32_session asession);

		// Recreate the logical session mapping after reconnecting to the same peer.
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient);

		// Feed one fully reconstructed KCP payload into the normal protocol pipeline.
		bool sempack(const std::shared_ptr<kcp_endpoint>& apstruct, const char* abuff, int abufflen);

		// Start the asynchronous UDP receive loop.
		void start();

		/*bool send(i32_sessionid asessionid, const char* buf, int len);

		int send(const basio::ip::udp::endpoint& aendpoint, const char* buf, int len);

		int sendbuff(i32_session asession, const char* buf, int len);

		int sendbuff(const basio::ip::udp::endpoint& aendpoint, const char* buf, int len);*/
	};
}// namespace ngl
