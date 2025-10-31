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
#include "asio_base.h"
#include "asio_tcp.h"

namespace ngl
{
	basio_ioservice* serviceio_info::get_ioservice(i32_threadid athreadid)
	{
		return std::get<0>(m_ioservices[athreadid]);
	}

	basio_ioservicework* serviceio_info::get_ioservice_work(i32_threadid athreadid)
	{
		return std::get<1>(m_ioservices[athreadid]);
	}

	serviceio_info::serviceio_info(i32_threadid athread, int32_t abuffmaxsize) :
		m_buffmaxsize(abuffmaxsize),
		m_recvthreadsize(athread),
		m_next_index(0)
	{
		for (int32_t i = 0; i < m_recvthreadsize + 1; ++i)
		{
			basio_ioservice* lpioservice = new basio_ioservice();
			basio_ioservicework* lpwork = new basio_ioservicework(*lpioservice);
			m_ioservices.push_back(std::make_tuple(lpioservice, lpwork, new std::thread([lpioservice]() {lpioservice->run(); })));
		}
	}

	serviceio_info::~serviceio_info()
	{
		for (int32_t i = 0; i < m_ioservices.size(); ++i)
		{
			delete std::get<2>(m_ioservices[i]);
			delete std::get<1>(m_ioservices[i]);
			delete std::get<0>(m_ioservices[i]);
		}
	}

	service_io::service_io(serviceio_info& amsi, i32_session asessionid) :
		m_threadid(amsi.m_next_index++ % amsi.m_recvthreadsize),
		m_ioservice(*(amsi.get_ioservice(m_threadid))),
		m_buff1(new char[amsi.m_buffmaxsize]),
		m_buff2(new char[amsi.m_buffmaxsize]),
		m_sessionid(asessionid),
		m_is_lanip(false),
		m_issend(false)
	{
	}

	service_io::~service_io()
	{
		delete[]m_buff1;
		delete[]m_buff2;
	}

	service_tcp::service_tcp(serviceio_info& amsi, i32_session asessionid) :
		service_io(amsi, asessionid),
		m_socket(m_ioservice)
	{
	}

	service_tcp::~service_tcp()
	{
	}
}// namespace ngl