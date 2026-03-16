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
#include "tools/threadtools.h"
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

		std::shared_ptr<basio_tcpacceptor>	m_acceptor_v4	= nullptr;		// Used tosupportipv4
		std::shared_ptr<basio_tcpacceptor>	m_acceptor_v6	= nullptr;		// Used tosupportipv6
		i16_port							m_port			= 0;			// Port
		tcp_callback						m_fun			= nullptr;		// Data callback
		tcp_closecallback					m_closefun		= nullptr;		// Closeconnection callback
		tcp_sendfinishcallback				m_sendfinishfun = nullptr;		// Send callback
		i32_sessionid						m_sessionid		= 0;			// Session id
		std::shared_mutex					m_maplock;						// Used tolock "m_data,m_sessionid"
		serviceio_info						m_service_ios;					// Asiosupport
		std::shared_mutex					m_ipportlock;					// Used tolock "m_ipport"
		map_service_tcp						m_data;							// Key:session id value:connectiondata
		map_ipport							m_ipport;						// key:session id value:ipport
		map_close							m_close;						// Closeconnectioncallback
	public:
		enum
		{
			etcp_buffmaxsize = 20480,	// tcp buff byte
			etcp_connect_interval = 1,	// Connection,
		};

		friend class service_tcp;

		// # Server server( port, connection)
		asio_tcp(
			i16_port aport										// Port
			, i32_threadsize athread							// Thread
			, const tcp_callback& acallfun						// Callback
			, const tcp_closecallback& aclosefun				// Closecallback
			, const tcp_sendfinishcallback& asendfinishfun		// Send callback
		);

		// # Client client(local port)
		asio_tcp(
			i32_threadsize athread								// Thread
			, const tcp_callback& acallfun						// Callback
			, const tcp_closecallback& aclosefun				// Closecallback
			, const tcp_sendfinishcallback& asendfinishfun		// Send callback
		);

		~asio_tcp();
	private:
		std::shared_ptr<service_tcp> get_tcp(i32_sessionid asessionid);

		template <typename T>
		bool spack(i32_sessionid asessionid, std::shared_ptr<T>& apack);

		template <typename TPACK>
		void async_send(const std::shared_ptr<service_tcp>& atcp, const std::shared_ptr<std::list<node_pack>>& alist, std::shared_ptr<TPACK>& apack, char* abuff, int32_t abufflen);

		void do_send(const std::shared_ptr<service_tcp>& atcp, const std::shared_ptr<std::list<node_pack>>& alist);

		void handle_write(const std::shared_ptr<service_tcp>& atcp, const basio_errorcode& error, std::shared_ptr<pack> apack);

		void handle_write(const std::shared_ptr<service_tcp>& atcp, const basio_errorcode& error, std::shared_ptr<void> apack);

		void close_socket(basio_iptcpsocket& socket);

		void accept_handle(bool aisv4, const std::shared_ptr<service_tcp>& aservice, const basio_errorcode& error);

		void accept(bool av4);

		void start(const std::shared_ptr<service_tcp>& aservice);
	public:
		// # Connection
		service_tcp* connect(const str_ip& aip, i16_port aport, const tcp_connectcallback& afun, int acount = 5);

		// # Sendpack
		bool send(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool send(i32_sessionid asessionid, std::shared_ptr<void>& apack);

		// # Closeconnection( notify )
		void close(i32_sessionid sessionid);
		void close(service_tcp* atcp);

		// # Closeconnection( notify )
		void close_net(i32_sessionid sessionid);

		// # Sessiongetipandport
		bool get_ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair);

		// # Setconnectionclosecallback
		void set_close(i32_sessionid asession, const std::function<void()>& afun);
	};	
}// namespace ngl