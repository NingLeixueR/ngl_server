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
// File overview: Declares interfaces for tcp.

#pragma once

#include "tools/serialize/pack.h"
#include "tools/tools/tools_thread.h"
#include "tools/log/nlog.h"
#include "net/asio_base.h"

namespace ngl
{
	using tcp_callback				= std::function<bool(service_tcp*, const char*, uint32_t)>;
	using tcp_closecallback			= std::function<void(i32_sessionid)>;
	using tcp_connectcallback		= std::function<void(i32_sessionid)>;
	using tcp_sendfinishcallback	= std::function<void(i32_sessionid, bool, const pack*)>;

	class asio_tcp
	{
		asio_tcp() = delete;
		asio_tcp(const asio_tcp&) = delete;
		asio_tcp& operator=(const asio_tcp&) = delete;

		using map_service_tcp = std::unordered_map<i32_sessionid, std::shared_ptr<service_tcp>>;
		using map_ipport = std::unordered_map<i32_sessionid, std::pair<str_ip, i16_port>>;
		using map_close = std::unordered_map<i32_sessionid, std::function<void()>>;

		std::shared_ptr<basio_tcpacceptor>	m_acceptor_v4	= nullptr;		// IPv4 listener for server mode.
		std::shared_ptr<basio_tcpacceptor>	m_acceptor_v6	= nullptr;		// IPv6 listener for server mode.
		std::mutex							m_acceptorlock;					// Serializes acceptor close/reset against async_accept re-arming.
		i16_port							m_port			= 0;			// Bound listen port, or 0 in client-only mode.
		tcp_callback						m_fun			= nullptr;		// Read callback for received bytes.
		tcp_closecallback					m_closefun		= nullptr;		// Global close notification.
		tcp_sendfinishcallback				m_sendfinishfun = nullptr;		// Per-send completion notification.
		i32_sessionid						m_sessionid		= 0;			// Monotonic session id allocator.
		std::shared_mutex					m_maplock;						// Protects m_data, m_close, and m_sessionid.
		serviceio_info						m_service_ios;					// io_context pool backing TCP sessions.
		std::shared_mutex					m_ipportlock;					// Protects m_ipport.
		map_service_tcp						m_data;							// Session id -> service_tcp object.
		map_ipport							m_ipport;						// Session id -> remote ip/port.
		map_close							m_close;						// Session-local reconnect/cleanup callbacks.
		std::shared_ptr<std::shared_mutex>	m_callbacklock = std::make_shared<std::shared_mutex>(); // Gates async callbacks against teardown.
		std::shared_ptr<std::atomic_bool>	m_alive = std::make_shared<std::atomic_bool>(true); // Shared liveness flag for async callbacks.
	public:
		enum
		{
			etcp_buffmaxsize = 20480,	// tcp buff byte
			etcp_connect_interval = 1,	// Connection,
		};

		friend class service_tcp;

		// Construct a TCP server that listens on the given port.
		asio_tcp(
			i16_port aport
			, i32_threadsize athread
			, const tcp_callback& acallfun
			, const tcp_closecallback& aclosefun
			, const tcp_sendfinishcallback& asendfinishfun
		);

		// Construct a TCP client pool with no listening socket.
		asio_tcp(
			i32_threadsize athread
			, const tcp_callback& acallfun
			, const tcp_closecallback& aclosefun
			, const tcp_sendfinishcallback& asendfinishfun
		);

		~asio_tcp() noexcept;
	private:
		// Resolve a session object and manage the serialized async send queue.
		std::shared_ptr<service_tcp> get_tcp(i32_sessionid asessionid);

		template <typename T>
		bool spack(i32_sessionid asessionid, std::shared_ptr<T>& apack);

		void async_send(const std::shared_ptr<service_tcp>& atcp, const std::shared_ptr<pack>& apack);
		void async_send(const std::shared_ptr<service_tcp>& atcp, const std::shared_ptr<void>& apack);

		void do_send(const std::shared_ptr<service_tcp>& atcp);

		void handle_write(const std::shared_ptr<service_tcp>& atcp, const basio_errorcode& error, std::shared_ptr<pack> apack);

		void handle_write(const std::shared_ptr<service_tcp>& atcp, const basio_errorcode& error, std::shared_ptr<void> apack);

		void close_socket(basio_iptcpsocket& socket);

		void accept_handle(bool aisv4, const std::shared_ptr<service_tcp>& aservice, const basio_errorcode& error);

		void accept(bool av4);

		void start(const std::shared_ptr<service_tcp>& aservice);
	public:
		i16_port port() const
		{
			return m_port;
		}

		// Start an outbound TCP connect with optional retry count.
		service_tcp* connect(const str_ip& aip, i16_port aport, const tcp_connectcallback& afun, int acount = 5);

		// Enqueue typed or type-erased packs for async send.
		bool send(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool send(i32_sessionid asessionid, std::shared_ptr<void>& apack);

		// Close a session and optionally notify callbacks.
		void close(i32_sessionid sessionid);
		void close(service_tcp* atcp);

		// Close a session without firing higher-level callbacks.
		void close_net(i32_sessionid sessionid);

		// Inspect peer endpoint info and register one per-session close callback.
		bool get_ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair);

		void set_close(i32_sessionid asession, const std::function<void()>& afun);
	};	
}// namespace ngl
