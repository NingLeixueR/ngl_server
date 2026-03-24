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
// File overview: Declares interfaces for net.

#pragma once

#include "tools/threadtools.h"
#include "net/node_pack.h"
#include "tools/type.h"

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <atomic>
#include <string>
#include <set>
#include <variant>

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
		bool					m_issend = false;	// Whether one async send is currently in flight.
		std::deque<node_pack>	m_list;				// Pending send queue drained by async completion handlers.
		std::mutex				m_mutex;
		basio_ioservice&		m_ioservice;

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

	class ws_send_node
	{
		ws_send_node() = delete;

		std::shared_ptr<pack> m_pack = nullptr;        // Binary pack with typed ownership.
		std::shared_ptr<void> m_voidpack = nullptr;    // Binary pack with erased ownership.
		std::shared_ptr<std::string> m_text = nullptr; // Text WebSocket payload.
	public:
		explicit ws_send_node(std::shared_ptr<pack>& apack) :
			m_pack(apack)
		{
		}

		explicit ws_send_node(std::shared_ptr<void>& apack) :
			m_voidpack(apack)
		{
		}

		explicit ws_send_node(const std::shared_ptr<std::string>& atext) :
			m_text(atext)
		{
		}

		bool is_pack() const
		{
			return m_pack != nullptr;
		}

		bool is_voidpack() const
		{
			return m_voidpack != nullptr;
		}

		bool is_text() const
		{
			return m_text != nullptr;
		}

		const std::shared_ptr<pack>& get_pack() const
		{
			return m_pack;
		}

		const std::shared_ptr<void>& get_voidpack() const
		{
			return m_voidpack;
		}

		const std::shared_ptr<std::string>& get_text() const
		{
			return m_text;
		}

		pack* get() const
		{
			if (m_pack != nullptr)
			{
				return m_pack.get();
			}
			if (m_voidpack != nullptr)
			{
				return static_cast<pack*>(m_voidpack.get());
			}
			return nullptr;
		}
	};

	class service_ws : public service_io
	{
		service_ws() = delete;

		using ws_stream_variant = std::variant<basio_websocket, basio_websocket_tls>;

		ws_stream_variant m_stream;       // Plain or TLS websocket stream.
		beast::flat_buffer m_read_buffer; // Owns websocket frame bytes between async reads.
		bool m_message_is_text = false;   // Current outbound/inbound opcode mode.
		bool m_use_tls = false;           // Whether the stream variant uses TLS.
	public:
		std::atomic_bool m_closing = false; // Prevent duplicate close sequences.
		bool m_ws_sending = false;          // Whether one async WS write is currently in flight.
		std::deque<ws_send_node> m_ws_send_list; // Pending websocket sends.

		service_ws(serviceio_info& amsi, i32_session asessionid);
		service_ws(serviceio_info& amsi, i32_session asessionid, basio_sslcontext& acontext);
		virtual ~service_ws() = default;

		// Access the underlying TCP socket regardless of plain/TLS websocket mode.
		basio_iptcpsocket& socket();
		const basio_iptcpsocket& socket() const;
		// Access per-connection websocket mode and read buffer state.
		bool using_tls() const;
		bool message_is_text() const;
		void set_message_is_text(bool avalue);
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
