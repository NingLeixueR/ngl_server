#pragma once

#include "threadtools.h"
#include "node_pack.h"
#include "type.h"

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <asio.hpp>
#include <atomic>
#include <string>
#include <set>

namespace ngl
{
	using basio_ioservice		= asio::io_service;
	using basio_ioservicework	= asio::io_service::work;
	using basio_iptcpsocket		= asio::ip::tcp::socket;
	using basio_iptcpendpoint	= asio::ip::tcp::endpoint;
	using basio_ipaddress		= asio::ip::address;
	using tuple_ioservice		= std::tuple<basio_ioservice*, basio_ioservicework*, std::thread*>;

	struct serviceio_info
	{
		serviceio_info() = delete;

		std::vector<tuple_ioservice>	m_ioservices;
		int32_t							m_next_index;
		int32_t							m_recvthreadsize;
		int32_t							m_buffmaxsize;

		basio_ioservice* get_ioservice(i32_threadid athreadid);
		basio_ioservicework* get_ioservice_work(i32_threadid athreadid);
		serviceio_info(i32_threadid athread, int32_t abuffmaxsize);
		~serviceio_info();
	};

	class service_io
	{
		service_io() = delete;
	public:
		i32_threadid			m_threadid;
		i32_sessionid			m_sessionid;
		basio_ioservice&		m_ioservice;
		char*					m_buff1;
		char*					m_buff2;
		bool					m_is_lanip;
		std::shared_mutex		m_mutex;
		bool					m_issend;	// �Ƿ���״̬
		std::list<node_pack>	m_list;		// ���Ͷ���(��Ϊasio�첽����,������û��ִ������ٴε���)

		service_io(serviceio_info& amsi, i32_session asessionid);
		virtual ~service_io();
	};

	class service_tcp : public service_io
	{
		service_tcp() = delete;
	public:
		basio_iptcpsocket m_socket;

		service_tcp(serviceio_info& amsi, i32_session asessionid);
		virtual ~service_tcp();
	};
}// namespace ngl
