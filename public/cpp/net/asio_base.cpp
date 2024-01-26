#include "asio_base.h"
#include "asio_tcp.h"

namespace ngl
{
	std::atomic<int32_t> service_io::m_globalsessionid = 1;

	basio_ioservice* serviceio_info::get_ioservice(int athreadid)
	{
		return std::get<0>(m_ioservices[athreadid]);
	}

	basio_ioservicework* serviceio_info::get_ioservice_work(int athreadid)
	{
		return std::get<1>(m_ioservices[athreadid]);
	}

	serviceio_info::serviceio_info(uint32_t athread, uint32_t abuffmaxsize) :
		m_buffmaxsize(abuffmaxsize),
		m_recvthreadsize(athread),
		m_next_index(0)
	{
		for (std::size_t i = 0; i < m_recvthreadsize + 1; ++i)
		{
			basio_ioservice* lpioservice = new basio_ioservice();
			basio_ioservicework* lpwork = new basio_ioservicework(*lpioservice);
			m_ioservices.push_back(std::make_tuple(lpioservice, lpwork, new std::thread([lpioservice]() {lpioservice->run(); })));
		}
	}

	serviceio_info::~serviceio_info()
	{
		for (int i = 0; i < m_ioservices.size(); ++i)
		{
			delete std::get<2>(m_ioservices[i]);
			delete std::get<1>(m_ioservices[i]);
			delete std::get<0>(m_ioservices[i]);
		}
	}

	service_io::service_io(serviceio_info& amsi, int asessionid) :
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

	i32_sessionid service_io::global_sessionid()
	{
		return m_globalsessionid.fetch_add(1);
	}

	service_tcp::service_tcp(serviceio_info& amsi, int asessionid) :
		service_io(amsi, asessionid),
		m_socket(m_ioservice)
	{
		//std::cout << "service_io [" << m_threadid << "]" << std::endl;
	}

	service_tcp::~service_tcp()
	{
	}

	basio_iptcpsocket& service_ws::socket()
	{
		return m_ws.next_layer().socket();
	}

	service_ws::service_ws(serviceio_info& amsi, int asessionid) :
		service_io(amsi, asessionid),
		m_ws(basio_iptcpsocket(*(amsi.get_ioservice(m_threadid))))
	{
		m_ws.binary(true);
	}


	service_ws::~service_ws()
	{
	}
}