#pragma once

#include "threadtools.h"
#include "node_pack.h"
#include "type.h"

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <atomic>
#include <string>
#include <set>

namespace ngl
{
	using basio_ioservice		= boost::asio::io_service;
	using basio_ioservicework	= boost::asio::io_service::work;
	using basio_iptcpsocket		= boost::asio::ip::tcp::socket;
	using basio_iptcpendpoint	= boost::asio::ip::tcp::endpoint;
	using basio_ipaddress		= boost::asio::ip::address;
	using tuple_ioservice		= std::tuple<basio_ioservice*, basio_ioservicework*, std::thread*>;

	using basio_websocket = boost::beast::websocket::stream<boost::beast::tcp_stream>;

	struct serviceio_info
	{
		std::vector<tuple_ioservice>	m_ioservices;
		uint32_t						m_next_index;
		uint32_t						m_recvthreadsize;
		uint32_t						m_buffmaxsize;

		basio_ioservice* get_ioservice(int athreadid);
		basio_ioservicework* get_ioservice_work(int athreadid);
		serviceio_info(uint32_t athread, uint32_t abuffmaxsize);
		~serviceio_info();
	};

	class service_io
	{
		static std::atomic<int32_t> m_globalsessionid;
	public:
		i32_threadid			m_threadid;
		i32_sessionid			m_sessionid;
		basio_ioservice&		m_ioservice;
		char*					m_buff1;
		char*					m_buff2;
		bool					m_is_lanip;
		std::shared_mutex		m_mutex;
		bool					m_issend;
		std::list<node_pack>	m_list; // 发送队列(因为asio异步操作,不能在没有执行完成再次调用)

		static i32_sessionid global_sessionid();

		service_io(serviceio_info& amsi, int asessionid);
		virtual ~service_io();
	};

	class service_tcp : public service_io
	{
	public:
		basio_iptcpsocket m_socket;

		service_tcp(serviceio_info& amsi, int asessionid);
		virtual ~service_tcp();
	};

	class service_ws : public service_io
	{
	public:
		basio_websocket m_ws;
		
		basio_iptcpsocket& socket();

		service_ws(serviceio_info& amsi, int asessionid);
		~service_ws();
	};
}// namespace ngl
