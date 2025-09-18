#include "sysconfig.h"
#include "asio_kcp.h"
#include "protocol.h"
#include "cmd.h"

#define USE_WHEEL_TIMER

namespace ngl
{
	//## [udp_cmd::ecmd_connect]		����aconnectΪtrue
	//## [udp_cmd::ecmd_connect_ret]	����aconnectΪfalse
	bool asio_kcp::function_econnect(ptr_se& apstruct, i64_actorid aactorid, bool aconnect)
	{
		i32_sessionid session = apstruct->m_session;
		xmlinfo const* linfo = nconfig::get_publicconfig();
		if (linfo == nullptr)
			return false;
		if (aconnect)
		{
			apstruct->m_actorid = aactorid;
#ifdef USE_WHEEL_TIMER 
			wheel_parm lparm
			{
				.m_ms = sysconfig::kcpping() * 1000,
				.m_intervalms = [](int64_t) {return sysconfig::kcpping() * 1000; } ,
				.m_count = 0x7fffffff,
				.m_fun = [session, this](const wheel_node* anode)
				{
					ptr_se lpstruct = m_session.find(session);
					if (lpstruct == nullptr)
					{
						m_kcptimer.removetimer(anode->m_timerid);
						return;
					}
					auto lnow = (int32_t)localtime::gettime();
					if (lnow - lpstruct->m_pingtm > sysconfig::kcppinginterval())
					{
						close(lpstruct->m_session);
						m_kcptimer.removetimer(anode->m_timerid);
					}
				}
			};
			apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
#endif//USE_WHEEL_TIMER
		}
		else
		{
#ifdef USE_WHEEL_TIMER 
			wheel_parm lparm
			{
				.m_ms = sysconfig::kcpping() * 1000,
				.m_intervalms = [](int64_t) {return sysconfig::kcpping() * 1000; } ,
				.m_count = 0x7fffffff,
				.m_fun = [session, this](const wheel_node* anode)
				{
					if (ptr_se lpstruct = m_session.find(session); lpstruct == nullptr)
					{
						m_kcptimer.removetimer(anode->m_timerid);
						return;
					}
					udp_cmd::sendcmd(this, session, udp_cmd::ecmd_ping, "");
				}
			};
			apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
#endif//USE_WHEEL_TIMER
		}
		return true;
	}
	//## udp_cmd::ecmd_connect
	void asio_kcp::function_ecmd_connect()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_connect, [](asio_kcp* ap, ptr_se& apstruct, const std::string& ajson)
			{
				apstruct->m_isconnect = true;
				apstruct->m_pingtm = (int)localtime::gettime();
				njson_read ltempjson(ajson.c_str());

				i64_actorid lactorid;
				if (!njson::read(ltempjson, "actorid", lactorid))
				{
					return;
				}
				std::string lsession;
				if (!njson::read(ltempjson, "session", lsession))
				{
					return;
				}
				if (ukcp::check_session(lactorid, lsession) == false)
				{
					return;
				}

				apstruct->m_actorid = lactorid;

				log_error()->print("kcp connect : {}@{}", session_endpoint::ip(apstruct.get()), session_endpoint::port(apstruct.get()));

				if (ap->function_econnect(apstruct, lactorid, true))
				{
					udp_cmd::sendcmd(ap, apstruct->m_session, udp_cmd::ecmd_connect_ret, "");
				}
			});
	}
	//## udp_cmd::ecmd_connect_ret
	void asio_kcp::function_ecmd_connect_ret()const
	{
		// ����robot ������������������ʱping 
		//if (nconfig::m_nodetype != ngl::ROBOT)
		//	return;
		udp_cmd::register_fun(udp_cmd::ecmd_connect_ret, [](asio_kcp* ap, ptr_se& apstruct, const std::string& ajson)
			{
				apstruct->m_isconnect = true;
				apstruct->m_pingtm = (int)localtime::gettime();
				// ��ʱ����cmd:ping
				ap->function_econnect(apstruct, -1, false);
				ap->m_connectfun(apstruct->m_session);
			});
	}
	//## udp_cmd::ecmd_ping
	void asio_kcp::asio_kcp::function_ecmd_ping()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_ping, [](asio_kcp* ap, ptr_se& apstruct, const std::string&)
			{
				apstruct->m_pingtm = (int)localtime::gettime();
			});
	}
	//## udp_cmd::ecmd_close
	void asio_kcp::function_ecmd_close()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_close, [](asio_kcp* ap, ptr_se& apstruct, const std::string&)
			{
				udp_cmd::sendcmd(ap, apstruct->m_session, udp_cmd::ecmd_close_ret, "");
				int lession = apstruct->m_session;
				wheel_parm lparm
				{
					.m_ms = 1000,
					.m_intervalms = [](int64_t) {return 1000; } ,
					.m_count = 0x7fffffff,
					.m_fun = [ap,lession](const wheel_node*)
					{
						ap->close(lession);
					}
				};
				apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
			});
	}

	asio_kcp::asio_kcp(i16_port port)
		: m_context()
		, m_port(port)
		, m_socket(m_context, asio_udp_endpoint(asio_udp::v4(), port))
		, m_session(this)
	{

		function_ecmd_connect();
		function_ecmd_connect_ret();
		function_ecmd_ping();
		function_ecmd_close();

		new thread([this]()
			{
				m_context.run();
			});
		start();
	}

	asio_kcp::~asio_kcp() {}

	bool asio_kcp::sempack(const ptr_se& apstruct, const char* abuff, int abufflen)
	{
		// ��ȡ��ͷ
		std::shared_ptr<pack> lpack = pack::make_pack(&m_pool, 0);
		lpack->m_protocol = ENET_KCP;
		lpack->m_id = apstruct->m_session;
		lpack->m_head.set_requestactor(apstruct->m_actorid);
		//lpack->m_segpack = m_segpack;
		if (EPH_HEAD_SUCCESS != lpack->m_head.push(abuff, abufflen))
		{
			return false;
		}
		int len = lpack->m_head.getvalue(EPH_BYTES);
		if (len < 0)
		{
			return false;
		}
		if (len != abufflen)
		{
			return false;
		}
		lpack->malloc(len);
		memcpy(lpack->m_buff, abuff, len);
		lpack->m_pos = len;
		if (localtime::gettime() < lpack->m_head.getvalue(EPH_TIME) + sysconfig::net_timeout())
		{
			protocol::push(lpack);
		}
		else
		{
			log_error()->print("time[{} < {} + {} ]"
				, localtime::gettime(), lpack->m_head.getvalue(EPH_TIME), sysconfig::net_timeout()
			);
		}
		return true;
	}

	void asio_kcp::start()
	{
		m_socket.async_receive_from(asio::buffer(m_buff, 1500), m_remoteport,
			[this](const std::error_code& ec, std::size_t bytes_received)
			{
				m_bytes_received = bytes_received;
				if (!ec && bytes_received > 0)
				{
					if (m_wait != nullptr && m_remoteport == m_waitendpoint)
					{
						m_wait(m_buff, (int)bytes_received);
						m_waitendpoint = asio_udp_endpoint();
						m_wait = nullptr;
					}
					else
					{
						ptr_se lpstruct = m_session.find(m_remoteport);
						if (lpstruct != nullptr)
						{
							int linput = lpstruct->input(m_buff, (long)bytes_received);
							if (linput >= 0)
							{
								while (true)
								{
									//�� buf�� ��ȡ�������ݣ�������ȡ�������ݴ�С
									int lrecv = lpstruct->recv(m_buffrecv, 10240);
									if (lrecv == -3)
									{
										// ret == -3 m_buffrecv �Ĵ�С���� 
										close(lpstruct->m_session);
										break;
									}
									if (lrecv < 0)
									{
										break;
									}

									// �����ж����Ƿ�kcp_cmd
									if (udp_cmd::run_cmd(this, lpstruct, m_buffrecv, lrecv))
									{
										log_error()->print("kcp cmd [{}]", std::string(m_buffrecv, lrecv));
										break;
									}

									if (lpstruct->m_isconnect == false)
									{
										break;
									}

									if (sempack(lpstruct, m_buffrecv, lrecv) == false)
									{
										close(lpstruct->m_session);
										break;
									}
								}
							}
							else
							{
								if (memcmp(m_buff, "GetIp", sizeof("GetIp") - 1) == 0)
								{
									std::string lip = m_remoteport.address().to_string();
									sendu(m_remoteport, lip.c_str(), (int)lip.size() + 1);
								}
							}
						}
						else
						{
							if (memcmp(m_buff, "GetIp", sizeof("GetIp") - 1) == 0)
							{
								std::string lip = m_remoteport.address().to_string();
								sendu(m_remoteport, lip.c_str(), (int)lip.size() + 1);
							}
							else
							{
								//NFC = not find connect
								sendu(m_remoteport, "NFC", sizeof("NFC"));
							}
						}
					}
					start();
				}
			});
	};

	// ## ����ԭʼudp��
	bool asio_kcp::sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len)
	{
		m_socket.async_send_to(asio::buffer(buf, len), aendpoint, [](const std::error_code& ec, std::size_t bytes_received)
			{
				if (ec)
				{
					log_error()->print("sendu err [{}]", ec.message());
				}
			});
		return true;
	}

	bool asio_kcp::sendu_waitrecv(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun)
	{
		m_wait = afun;
		m_waitendpoint = aendpoint;
		m_socket.async_send_to(asio::buffer(buf, len), aendpoint, [this, aendpoint, buf, len, afun](const std::error_code& ec, std::size_t bytes_received)
			{
				if (ec)
				{
					log_error()->print("async_send_to err [{}]", ec.message());
					wheel_parm lparm
					{
						.m_ms = 1000,
						.m_intervalms = [](int64_t) {return 1000; } ,
						.m_count = 1,
						.m_fun = [this, aendpoint, buf, len, afun](const wheel_node*)
						{
							sendu_waitrecv(aendpoint, buf, len, afun);
						}
					};
					m_kcptimer.addtimer(lparm);
				}
			});
		return true;
	}

	// ## ͨ��kcp����pack
	bool asio_kcp::sendpack(i32_sessionid asessionid, const std::shared_ptr<pack>& apack)
	{
		ptr_se lpstruct = m_session.find(asessionid);
		if (lpstruct == nullptr)
		{
			return false;
		}

		send(lpstruct->m_endpoint, apack->m_buff, apack->m_len);
		return true;
	}

	// ## ͨ��kcp����pack
	bool asio_kcp::sendpack(const asio_udp_endpoint& aendpoint, const std::shared_ptr<pack>& apack)
	{
		send(aendpoint, apack->m_buff, apack->m_len);
		return true;
	}

	bool asio_kcp::send(i32_sessionid asessionid, const char* buf, int len)
	{
		ptr_se lpstruct = m_session.find(asessionid);
		if (lpstruct == nullptr)
		{
			return false;
		}

		send(lpstruct->m_endpoint, buf, len);
		return true;
	}

	int asio_kcp::send(const asio_udp_endpoint& aendpoint, const char* buf, int len)
	{
		ptr_se lpstruct = m_session.find(aendpoint);
		if (lpstruct == nullptr)
		{
			return -1;
		}
		int ret = lpstruct->send(buf, len);
		// ����flushһ�� �Ը����ÿͻ����յ�����
		lpstruct->flush();
		return ret;
	}

	// ## kcp���ͻص�����õķ���
	int asio_kcp::sendbuff(i32_session asession, const char* buf, int len)
	{
		ptr_se lpstruct = m_session.find(asession);
		m_socket.async_send_to(asio::buffer(buf, len), lpstruct->m_endpoint, [](const std::error_code& ec, std::size_t)
			{
				if (ec)
				{
					log_error()->print("impl_asio_kcp::sendbuff error [{}]", ec.message());
				}
			});
		return 0;
	}

	int asio_kcp::sendbuff(const asio_udp_endpoint& aendpoint, const char* buf, int len)
	{
		m_socket.async_send_to(asio::buffer(buf, len), aendpoint, [](const std::error_code& ec, std::size_t)
			{
				if (ec)
				{
					log_error()->print("impl_asio_kcp::sendbuff error [{}]", ec.message());
				}
			});
		return 0;
	}

	void asio_kcp::connect(int32_t aconv
		, std::string& akcpsess
		, i64_actorid aactorid
		, const std::string& aip
		, i16_port aport
		, const std::function<void(i32_session)>& afun
	)
	{
		ngl::asio_udp_endpoint lendpoint(asio::ip::address::from_string(aip), aport);
		connect(aconv, akcpsess, aactorid, lendpoint, afun);
	}

	void asio_kcp::connect(int32_t aconv
		, std::string& akcpsess
		, i64_actorid aactorid
		, const asio_udp_endpoint& aendpoint
		, const std::function<void(i32_session)>& afun
	)
	{
		// #### ��������
		ptr_se lpstruct = m_session.add(aconv, aendpoint, aactorid);
		njson_write ltempjson;
		njson::write(ltempjson
			, "actorid", aactorid
			, "session", akcpsess
		);
		ltempjson.set_nonformatstr(true);
		std::string lparm = ltempjson.get();
		udp_cmd::sendcmd(this, lpstruct->m_session, udp_cmd::ecmd_connect, lparm);
		m_connectfun = afun;
	}

	i64_actorid asio_kcp::find_actorid(i32_session asession)
	{
		ptr_se lpstruct = m_session.find(asession);
		if (lpstruct == nullptr)
		{
			return -1;
		}
		return lpstruct->m_actorid;
	}

	void asio_kcp::close(i32_session asession)
	{
		m_session.erase(asession);
	}

	void asio_kcp::reset_add(int32_t aconv, const std::string& aip, i16_port aport)
	{
		ngl::asio_udp_endpoint lendpoint(asio::ip::address::from_string(aip), aport);
		m_session.reset_add(aconv, lendpoint, -1);
	}

	void asio_kcp::close_net(i32_session asession)
	{
		close(asession);
	}
}// namespace ngl