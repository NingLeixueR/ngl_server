#include "asio_kcp.h"


namespace ngl
{
	time_wheel m_kcptimer(time_wheel_config
		{
			.m_time_wheel_precision = 5,
			.m_time_wheel_bit = 8,
			.m_time_wheel_count = 4,
		});

	int udp_output(const char* buf, int len, ikcpcb* kcp, void* user)
	{
		std::cout << "send###" << buf << std::endl;
		session_endpoint* lpstruct = (session_endpoint*)user;
		lpstruct->m_asiokcp->sendbuff(lpstruct->m_session, lpstruct->m_endpoint, buf, len);
		return len;
	}

	void nkcp::create(IUINT32 asessionid, void* auser)
	{
		std::shared_ptr<ikcpcb> ltemp(ikcp_create(1, auser), [auser](ikcpcb* aikcpcb)
			{
				((session_endpoint*)auser)->removetimer();
				ikcp_release(aikcpcb);
			});
		m_kcp = ltemp;
	}
}