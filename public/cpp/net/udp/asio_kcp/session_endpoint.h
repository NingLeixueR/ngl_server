#pragma once

#include "threadtools.h"
#include "time_wheel.h"
#include "asio_base.h"
#include "type.h"
#include "pack.h"
#include "nlog.h"
#include "impl.h"
#include "ikcp.h"
#include "impl.h"

#include <asio.hpp>
#include <memory>
#include <map>

namespace ngl
{
	using asio_udp = asio::ip::udp;
	using asio_udp_endpoint = asio_udp::endpoint;

	class asio_kcp;

	extern time_wheel m_kcptimer;

	struct session_endpoint
	{
		i32_sessionid		m_session;
		asio_udp_endpoint	m_endpoint;
		std::string			m_ip;
		i16_port			m_port;
		asio_kcp*			m_asiokcp;
		int64_t				m_timerid;
		ikcpcb*				m_kcp;
		bool				m_isconnect;		// 是否接收到kcp_cmd::ecmd_connect 或者ecmd_connect_ret
		int					m_pingtm;			// 进行ping计时 
		int64_t				m_pingtimerid;
		i64_actorid			m_actorid;

		typedef int (*output)(const char* buf, int len, struct IKCPCB* kcp, void* user);

		session_endpoint();

		~session_endpoint();

		static std::string ip(session_endpoint* ap);

		static i16_port port(session_endpoint* ap);

		void create(int32_t aconv, IUINT32 asessionid, void* auser);

		void removetimer();

		int setmtu(int mtu);

		void setoutput(output afun);

		int nodelay(int nodelay, int interval, int resend, int nc);

		int wndsize(int sndwnd, int rcvwnd);

		void update(IUINT32 current);

		int input(const char* data, long size);

		int recv(char* buffer, int len);

		int send(const char* buffer, int len);

		void flush();

		void release();
	};
}//namespace ngl