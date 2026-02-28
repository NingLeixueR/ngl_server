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

#include "net/tcp/asio_tcp.h"
#include "net/asio_base.h"

#include <algorithm>

namespace ngl
{
	basio_ioservice* serviceio_info::get_ioservice(i32_threadid athreadid)
	{
		if (m_ioservices.empty())
		{
			return nullptr;
		}
		if (athreadid < 0 || static_cast<size_t>(athreadid) >= m_ioservices.size())
		{
			return std::get<0>(m_ioservices[0]).get();
		}
		return std::get<0>(m_ioservices[athreadid]).get();
	}

	basio_ioservicework* serviceio_info::get_ioservice_work(i32_threadid athreadid)
	{
		if (m_ioservices.empty())
		{
			return nullptr;
		}
		if (athreadid < 0 || static_cast<size_t>(athreadid) >= m_ioservices.size())
		{
			return std::get<1>(m_ioservices[0]).get();
		}
		return std::get<1>(m_ioservices[athreadid]).get();
	}

	serviceio_info::serviceio_info(i32_threadid athread, int32_t abuffmaxsize) :
		m_buffmaxsize(abuffmaxsize),
		m_recvthreadsize(std::max<int32_t>(1, athread)),
		m_next_index(0)
	{
		// Ensure at least one io_service exists. create exactly m_recvthreadsize services.
		for (int32_t i = 0; i < m_recvthreadsize; ++i)
		{
			auto lpioservice = std::make_shared<basio_ioservice>();
			auto lpwork = std::make_shared<basio_ioservicework>(*lpioservice);
			m_ioservices.push_back(
				std::make_tuple(lpioservice, lpwork, std::make_shared<std::thread>([lpioservice]() {lpioservice->run(); }))
			);
		}
	}

	service_io::service_io(serviceio_info& amsi, i32_session asessionid) :
		m_threadid(amsi.m_next_index++ % amsi.m_recvthreadsize)
		, m_ioservice(*(amsi.get_ioservice(m_threadid)))
		, m_buff1(std::make_unique<char[]>(amsi.m_buffmaxsize))
		, m_buff2(std::make_unique<char[]>(amsi.m_buffmaxsize))
		, m_pbuff1(m_buff1.get())
		, m_pbuff2(m_buff2.get())
		, m_sessionid(asessionid)
		, m_is_lanip(false)
		, m_issend(false)
	{
	}

	char* service_io::buff()
	{
		std::swap(m_pbuff1, m_pbuff2);
		return m_pbuff1;
	}

	service_tcp::service_tcp(serviceio_info& amsi, i32_session asessionid) :
		service_io(amsi, asessionid),
		m_socket(m_ioservice)
	{
	}
}// namespace ngl