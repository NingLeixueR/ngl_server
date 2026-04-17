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
// File overview: Asio I/O context wrapper shared across all transport layers.

#pragma once

#include "tools/tools/tools_thread.h"
#include "net/node_pack.h"
#include "tools/type.h"

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <variant>
#include <atomic>
#include <string>
#include <deque>
#include <set>

namespace ngl
{
	namespace basio = boost::asio;
	namespace beast = boost::beast;
	namespace bwebsocket = beast::websocket;

	using basio_errorcode		= boost::system::error_code;
	using basio_ioservice		= basio::io_context;
	using basio_ioservicework	= basio::executor_work_guard<basio_ioservice::executor_type>;
	using basio_iptcpsocket		= basio::ip::tcp::socket;
	using basio_iptcpendpoint	= basio::ip::tcp::endpoint;
	using basio_ipaddress		= basio::ip::address;
	using tuple_ioservice		= std::tuple<std::shared_ptr<basio_ioservice>, std::shared_ptr<basio_ioservicework>, std::shared_ptr<std::thread>>;
	using basio_tcpacceptor		= basio::ip::tcp::acceptor;
	using basio_tcpstream		= beast::tcp_stream;
	using basio_sslcontext		= basio::ssl::context;
	using basio_sslstream		= basio::ssl::stream<basio_tcpstream>;
	using basio_websocket		= bwebsocket::stream<basio_tcpstream>;
	using basio_websocket_tls	= bwebsocket::stream<basio_sslstream>;

	class asio_ws;

	struct serviceio_info
	{
		serviceio_info() = delete;

		std::vector<tuple_ioservice>	m_ioservices;		// One io_context + work guard + thread per receive worker.
		int32_t							m_next_index = 0;	// Round-robin index for new sessions.
		int32_t							m_recvthreadsize = 0; // Number of receive io threads.
		int32_t							m_buffmaxsize = 0;	// Per-session scratch buffer size.

		// Resolve the io_context/work guard assigned to a specific receive thread.
		basio_ioservice* get_ioservice(i32_threadid athreadid);
		// Create the io_context thread pool used by TCP/WS sessions.
		serviceio_info(i32_threadid athread, int32_t abuffmaxsize);
		// Stop every io_context and join all worker threads.
		void shutdown();
		~serviceio_info();
	};

	class service_io
	{
		service_io() = delete;

		std::unique_ptr<char[]> m_buff1 = nullptr;
		std::unique_ptr<char[]> m_buff2 = nullptr;
		char* m_pbuff1 = nullptr;
		char* m_pbuff2 = nullptr;
	public:
		i32_threadid			m_threadid = 0;
		i32_sessionid			m_sessionid = 0;
		bool					m_is_lanip = false;
		basio_ioservice&		m_ioservice;
		npack_list				m_npacklist;

		service_io(serviceio_info& amsi, i32_session asessionid);
		virtual ~service_io() =default;

		// Swap between two scratch buffers so one can be consumed while the other is reused.
		char* buff();
	};

	class service_tcp : public service_io
	{
		service_tcp() = delete;
	public:
		basio_iptcpsocket m_socket;

		service_tcp(serviceio_info& amsi, i32_session asessionid);
		virtual ~service_tcp() = default;
	};

	class service_ws : public service_io
	{
		service_ws() = delete;

		using ws_stream_variant = std::variant<basio_websocket, basio_websocket_tls>;

		ws_stream_variant m_stream;       // Plain or TLS websocket stream.
		beast::flat_buffer m_read_buffer; // Owns websocket frame bytes between async reads.
		bool m_use_tls = false;           // Whether the stream variant uses TLS.
	public:
		service_ws(serviceio_info& amsi, i32_session asessionid);
		service_ws(serviceio_info& amsi, i32_session asessionid, basio_sslcontext& acontext);
		virtual ~service_ws() = default;

		// Access the underlying TCP socket regardless of plain/TLS websocket mode.
		basio_iptcpsocket& socket();
		const basio_iptcpsocket& socket() const;
		// Access per-connection websocket mode and read buffer state.
		bool using_tls() const;
		beast::flat_buffer& read_buffer();
		void consume_read_buffer(std::size_t asize);

		template <typename TFUN>
		decltype(auto) visit_stream(TFUN&& afun)
		{
			return std::visit(
				[&afun](auto& astream) -> decltype(auto)
				{
					return afun(astream);
				},
				m_stream
			);
		}

		template <typename TFUN>
		decltype(auto) visit_stream(TFUN&& afun) const
		{
			return std::visit(
				[&afun](const auto& astream) -> decltype(auto)
				{
					return afun(astream);
				},
				m_stream
			);
		}
	};
}// namespace ngl
