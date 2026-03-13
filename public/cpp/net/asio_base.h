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

#include "tools/threadtools.h"
#include "net/node_pack.h"
#include "tools/type.h"

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

		std::vector<tuple_ioservice>	m_ioservices;
		int32_t							m_next_index = 0;
		int32_t							m_recvthreadsize = 0;
		int32_t							m_buffmaxsize = 0;

		basio_ioservice* get_ioservice(i32_threadid athreadid);
		basio_ioservicework* get_ioservice_work(i32_threadid athreadid);
		serviceio_info(i32_threadid athread, int32_t abuffmaxsize);
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
		bool					m_issend = false;	// 是否发送状态
		std::list<node_pack>	m_list;				// 发送队列(因为asio异步操作,不能在没有执行完成再次调用)
		std::shared_mutex		m_mutex;
		basio_ioservice&		m_ioservice;

		service_io(serviceio_info& amsi, i32_session asessionid);
		virtual ~service_io() =default;

		// # 有两个buff指针交替使用
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

		std::shared_ptr<pack> m_pack = nullptr;
		std::shared_ptr<void> m_voidpack = nullptr;
		std::shared_ptr<std::string> m_text = nullptr;
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

		ws_stream_variant m_stream;
		beast::flat_buffer m_read_buffer;
		bool m_message_is_text = false;
		bool m_use_tls = false;
	public:
		bool m_ws_sending = false;
		std::list<ws_send_node> m_ws_send_list;

		service_ws(serviceio_info& amsi, i32_session asessionid);
		service_ws(serviceio_info& amsi, i32_session asessionid, basio_sslcontext& acontext);
		virtual ~service_ws() = default;

		basio_iptcpsocket& socket();
		const basio_iptcpsocket& socket() const;
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
