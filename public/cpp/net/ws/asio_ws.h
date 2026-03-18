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
// File overview: Declares interfaces for ws.

#pragma once

#include "tools/serialize/pack.h"
#include "tools/threadtools.h"
#include "tools/log/nlog.h"
#include "net/asio_base.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace ngl
{
	struct ws_tls_options
	{
		std::string m_certificate_chain; // PEM certificate chain for server mode.
		std::string m_private_key;       // PEM private key for server mode.
		std::string m_ca_certificates;   // Optional CA bundle for client verification.
		bool m_verify_peer = true;       // Whether TLS clients verify the remote certificate.
	};

	using ws_callback = std::function<bool(service_ws*, const char*, uint32_t)>;
	using ws_closecallback = std::function<void(i32_sessionid)>;
	using ws_connectcallback = std::function<void(i32_sessionid)>;
	using ws_sendfinishcallback = std::function<void(i32_sessionid, bool, const pack*)>;

	class asio_ws
	{
		asio_ws() = delete;
		asio_ws(const asio_ws&) = delete;
		asio_ws& operator=(const asio_ws&) = delete;

		using map_service_ws = std::unordered_map<i32_sessionid, std::shared_ptr<service_ws>>;
		using map_ipport = std::unordered_map<i32_sessionid, std::pair<str_ip, i16_port>>;
		using map_close = std::unordered_map<i32_sessionid, std::function<void()>>;

		std::shared_ptr<basio_tcpacceptor>	m_acceptor_v4 = nullptr;		// IPv4 listener for server mode.
		std::shared_ptr<basio_tcpacceptor>	m_acceptor_v6 = nullptr;		// IPv6 listener for server mode.
		i16_port							m_port = 0;						// Bound listen port, or 0 in client-only mode.
		bool								m_use_tls = false;				// True for WSS, false for plain WS.
		ws_callback							m_fun = nullptr;				// Read callback for received frames.
		ws_closecallback					m_closefun = nullptr;			// Global close notification.
		ws_sendfinishcallback				m_sendfinishfun = nullptr;		// Per-send completion notification.
		i32_sessionid						m_sessionid = 0;				// Monotonic session id allocator.
		serviceio_info						m_service_ios;					// io_context pool backing websocket sessions.
		std::shared_mutex					m_maplock;						// Protects m_data, m_close, and m_sessionid.
		std::shared_mutex					m_ipportlock;					// Protects m_ipport.
		map_service_ws						m_data;							// Session id -> service_ws object.
		map_ipport							m_ipport;						// Session id -> remote ip/port.
		map_close							m_close;						// Session-local reconnect/cleanup callbacks.
		std::unique_ptr<basio_sslcontext>	m_tls_context = nullptr;		// Shared TLS context when WSS is enabled.
		ws_tls_options						m_tls_options;					// TLS configuration.
		std::shared_ptr<std::atomic_bool>	m_alive = std::make_shared<std::atomic_bool>(true); // Shared liveness flag for async callbacks.
	public:
		enum
		{
			ews_connect_interval = 1,
			ews_default_recvbuff = 20480,
		};

		asio_ws(
			i16_port aport,
			i32_threadsize athread,
			bool ause_tls,
			const ws_callback& acallfun,
			const ws_closecallback& aclosefun,
			const ws_sendfinishcallback& asendfinishfun,
			const ws_tls_options& atls_options = {}
		);

		asio_ws(
			i32_threadsize athread,
			bool ause_tls,
			const ws_callback& acallfun,
			const ws_closecallback& aclosefun,
			const ws_sendfinishcallback& asendfinishfun,
			const ws_tls_options& atls_options = {}
		);

		~asio_ws();
	private:
		// Allocate/lookup sessions and run the TCP/TLS/WebSocket handshake pipeline.
		std::shared_ptr<service_ws> create_service();
		std::shared_ptr<service_ws> get_ws(i32_sessionid asessionid);
		void prepare_tls_context(bool aserver);
		void configure_websocket(const std::shared_ptr<service_ws>& aservice, bool aserver);
		bool cache_remote_endpoint(const std::shared_ptr<service_ws>& aservice);
		void handle_connect_failure(
			const std::shared_ptr<service_ws>& aservice,
			const basio_errorcode& aerror,
			const str_host& ahost,
			i16_port aport,
			const std::string& atarget,
			const ws_connectcallback& afun,
			int acount,
			const char* astage
		);
		void begin_server_handshake(const std::shared_ptr<service_ws>& aservice);
		void begin_client_handshake(
			const std::shared_ptr<service_ws>& aservice,
			const str_host& ahost,
			const std::string& atarget,
			const ws_connectcallback& afun,
			i16_port aport,
			int acount
		);
		// Enqueue frames, drive the serialized async-write chain, and manage closure.
		bool queue_send(i32_sessionid asessionid, const ws_send_node& anode);
		void do_send(const std::shared_ptr<service_ws>& aservice, const std::shared_ptr<std::list<ws_send_node>>& alist);
		void handle_write(const std::shared_ptr<service_ws>& aservice, const basio_errorcode& error, const ws_send_node& anode);
		void accept_handle(bool av4, const std::shared_ptr<service_ws>& aservice, const basio_errorcode& error);
		void accept(bool av4);
		void start(const std::shared_ptr<service_ws>& aservice);
		void close_session(i32_sessionid sessionid, bool agraceful, bool anotifyclose, bool anotifycallback);
		void close_socket(basio_iptcpsocket& asocket);
	public:
		i16_port port() const;

		// Start outbound connections, send frames, and inspect session metadata.
		service_ws* connect(const str_host& ahost, i16_port aport, const std::string& atarget, const ws_connectcallback& afun, int acount = 5);
		service_ws* connect(const str_host& ahost, i16_port aport, const ws_connectcallback& afun, int acount = 5);

		bool send(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool send(i32_sessionid asessionid, std::shared_ptr<void>& apack);
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack);
		bool send_msg(i32_sessionid asessionid, const std::string& amsg);

		void close(i32_sessionid sessionid);
		void close(service_ws* asession);
		void close_net(i32_sessionid sessionid);

		bool get_ipport(i32_sessionid asessionid, std::pair<str_ip, i16_port>& apair);
		bool sessionid2ipport(i32_sessionid asessionid, std::pair<str_ip, i16_port>& apair);
		bool exist_session(i32_sessionid asessionid);

		void set_close(i32_sessionid asession, const std::function<void()>& afun);
	};
}// namespace ngl
