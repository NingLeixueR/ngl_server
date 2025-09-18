#include "session_endpoint.h"
#include "asio_kcp.h"

namespace ngl
{
	time_wheel m_kcptimer(time_wheel_config
		{
			.m_time_wheel_precision = 5,
			.m_time_wheel_bit = 8,
			.m_time_wheel_count = 4,
		});

	session_endpoint::session_endpoint()
		: m_session(0)
		, m_port(0)
		, m_asiokcp(nullptr)
		, m_timerid(0)
		, m_isconnect(false)
		, m_pingtm(0)
		, m_pingtimerid(0)
		, m_actorid(-1)
		, m_kcp(nullptr)
	{}

	session_endpoint::~session_endpoint()
	{
		removetimer();
		release();
	}

	std::string session_endpoint::ip(session_endpoint* ap)
	{
		return ap->m_endpoint.address().to_string();
	}

	i16_port session_endpoint::port(session_endpoint* ap)
	{
		return ap->m_endpoint.port();
	}

	void session_endpoint::create(int32_t aconv, IUINT32 asessionid, void* auser)
	{
		m_kcp = ikcp_create(aconv, auser);
	}

	void session_endpoint::removetimer()
	{
		if (m_timerid != 0)
		{
			m_kcptimer.removetimer(m_timerid);
		}
		if (m_pingtimerid != 0)
		{
			m_kcptimer.removetimer(m_pingtimerid);
		}
	}

	int session_endpoint::setmtu(int mtu)
	{
		return ikcp_setmtu(m_kcp, mtu);
	}

	void session_endpoint::setoutput(output afun)
	{
		m_kcp->output = afun;
	}

	int session_endpoint::nodelay(int nodelay, int interval, int resend, int nc)
	{
		return ikcp_nodelay(m_kcp, nodelay, interval, resend, nc);
	}

	int session_endpoint::wndsize(int sndwnd, int rcvwnd)
	{
		return ikcp_wndsize(m_kcp, sndwnd, rcvwnd);
	}

	void session_endpoint::update(IUINT32 current)
	{
		ikcp_update(m_kcp, current);
	}

	int session_endpoint::input(const char* data, long size)
	{
		return ikcp_input(m_kcp, data, size);
	}

	int session_endpoint::recv(char* buffer, int len)
	{
		return ikcp_recv(m_kcp, buffer, len);
	}

	int session_endpoint::send(const char* buffer, int len)
	{
		return ikcp_send(m_kcp, buffer, len);
	}

	void session_endpoint::flush()
	{
		ikcp_flush(m_kcp);
	}

	void session_endpoint::release()
	{
		return ikcp_release(m_kcp);
	}

}//namespace ngl