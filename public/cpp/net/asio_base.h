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
	using tuple_ioservice		= std::tuple<std::shared_ptr<basio_ioservice>, std::shared_ptr<basio_ioservicework>, std::shared_ptr<std::thread>>;

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
		virtual ~service_io();

		char* swap_buff();
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