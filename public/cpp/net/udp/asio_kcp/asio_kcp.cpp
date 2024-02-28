#include "asio_kcp.h"
#include "protocol.h"


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
		//std::cout << "send###" << buf << std::endl;
		session_endpoint* lpstruct = (session_endpoint*)user;
		lpstruct->m_asiokcp->sendbuff(lpstruct->m_endpoint, buf, len);
		return len;
	}

	std::map<udp_cmd::ecmd, udp_cmd::ecmd_callback> udp_cmd::m_cmdfun;

	void udp_cmd::sendcmd(asio_kcp* akcp, i32_sessionid asession, udp_cmd::ecmd acmd, const std::string& ajson)
	{
		std::stringstream lstream;
		lstream << "ecmd*" << (int)acmd << "*" << ajson;
		std::string lstr = lstream.str();
		akcp->send(asession, lstr.c_str(), lstr.size()+1);
	}

	asio_kcp::asio_kcp(i16_port port)
		: m_context()
		, m_socket(m_context, asio_udp_endpoint(asio_udp::v4(), port))
		, m_session(this)
	{
		// 发起连接
		session_manage* lpsession = &m_session;

		auto lcallfun = [lpsession, this](ptr_se& apstruct)->bool
		{
				return true;
				// 定时监测连接是否可用
				if (nconfig::m_nodetype != ngl::ROBOT)
				{
					i32_sessionid session = apstruct->m_session;
					xmlinfo* linfo = nconfig::get_publicconfig();
					if (linfo == nullptr)
						return false;
					int ms = 0;
					if (linfo->find("kcp_ping", ms) == false)
						return false;
					int intervalms = 0;
					if (linfo->find("kcp_ping_interval", intervalms) == false)
						return false;
					wheel_parm lparm
					{
						.m_ms = intervalms,
						.m_intervalms = [intervalms](int64_t) {return intervalms; } ,
						.m_count = 0x7fffffff,
						.m_fun = [lpsession,session,ms,intervalms, this](wheel_node* anode)
						{
							ptr_se lpstruct = lpsession->find(session);
							if (lpstruct == nullptr)
							{
								m_kcptimer.removetimer(anode->m_timerid);
								return;
							}
							int32_t lnow = localtime::gettime();
							if (lnow - lpstruct->m_pingtm > intervalms)
							{
								close(lpstruct->m_session);
								m_kcptimer.removetimer(anode->m_timerid);
							}
						}
					};
					apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
				}
				else
				{
					i32_sessionid session = apstruct->m_session;
					xmlinfo* linfo = nconfig::get_publicconfig();
					if (linfo == nullptr)
						return false;
					int ms = 0;
					if (linfo->find("kcp_ping", ms) == false)
						return false;
					int intervalms = 0;
					if (linfo->find("kcp_ping_interval", intervalms) == false)
						return false;
					wheel_parm lparm
					{
						.m_ms = ms,
						.m_intervalms = [ms](int64_t) {return ms; } ,
						.m_count = 0x7fffffff,
						.m_fun = [lpsession,session,ms,intervalms, this](wheel_node* anode)
						{
							ptr_se lpstruct = lpsession->find(session);
							if (lpstruct == nullptr)
							{
								m_kcptimer.removetimer(anode->m_timerid);
								return;
							}
							udp_cmd::sendcmd(this, session, udp_cmd::ecmd_ping, "");
						}
					};
					apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
				}
				
				return true;
		};

		udp_cmd::register_fun(udp_cmd::ecmd_connect, [this, lcallfun](ptr_se& apstruct, const std::string& ajson)
			{
				apstruct->m_isconnect = true;
				apstruct->m_pingtm = localtime::gettime();
				ojson ltempjson(ajson.c_str());
				std::pair<const char*, i64_actorid> lactorpair("actorid", 0);
				if ((ltempjson >> lactorpair) == false)
					return;
				std::pair<const char*, std::string> lsessionpair("session", "");
				if ((ltempjson >> lsessionpair) == false)
					return;
				if (udp_kcp::check_session(lactorpair.second, lsessionpair.second) == false)
					return;

				apstruct->m_actorid = lactorpair.second;

				if (lcallfun(apstruct))
					udp_cmd::sendcmd(this, apstruct->m_session, udp_cmd::ecmd_connect_ret, "");
			});

		udp_cmd::register_fun(udp_cmd::ecmd_connect_ret, [lpsession, this, lcallfun](ptr_se& apstruct, const std::string& ajson)
			{
				apstruct->m_isconnect = true;
				apstruct->m_pingtm = localtime::gettime();

				// 定时发送cmd:ping

				lcallfun(apstruct);
				m_connectfun(apstruct->m_session);
			});

		udp_cmd::register_fun(udp_cmd::ecmd_ping, [lpsession, this, lcallfun](ptr_se& apstruct, const std::string& ajson)
			{
				apstruct->m_pingtm = localtime::gettime();
			});

		//ecmd_close
		udp_cmd::register_fun(udp_cmd::ecmd_close, [lpsession, this, lcallfun](ptr_se& apstruct, const std::string& ajson)
			{
				udp_cmd::sendcmd(this, apstruct->m_session, udp_cmd::ecmd_close_ret, "");
				int lession = apstruct->m_session;
				//apstruct->m_asiokcp->close(apstruct->m_session);
				wheel_parm lparm
				{
					.m_ms = 1000,
					.m_intervalms = [](int64_t) {return 1000; } ,
					.m_count = 0x7fffffff,
					.m_fun = [lpsession,this,lession](wheel_node* anode)
					{
						close(lession);
					}
				};
				apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
			});

		new thread([this]()
			{
				m_context.run();
			});
		start();
	}

	bool asio_kcp::sempack(ptr_se& apstruct, const char* abuff, int abufflen)
	{
		// 获取包头
		std::shared_ptr<pack> lpack = pack::make_pack(&m_pool, 0);
		lpack->m_protocol = ENET_KCP;
		lpack->m_id = apstruct->m_session;
		//lpack->m_segpack = m_segpack;
		EPH_HEAD_VAL lval = lpack->m_head.push(abuff, abufflen);
		if (EPH_HEAD_SUCCESS != lval)
			return false;
		int len = lpack->m_head.getvalue(EPH_BYTES);
		if (len < 0)
			return false;
		if (len != abufflen)
			return false;
		lpack->malloc(len);
		memcpy(lpack->m_buff, abuff, len);
		lpack->m_pos = len;
		if (localtime::gettime() < lpack->m_head.getvalue(EPH_TIME) + DEF_TIMEOUT_SECOND)
		{
			protocol::push(lpack);
		}
		else
		{
			LogLocalError("time[% < % + % ]", localtime::gettime(), lpack->m_head.getvalue(EPH_TIME), DEF_TIMEOUT_SECOND);
		}
		return true;
	}

	

	void asio_kcp::start()
	{
		m_socket.async_receive_from(boost::asio::buffer(m_buff, 1500), m_remoteport,
			[this](const boost::system::error_code& ec, std::size_t bytes_received)
			{
				m_bytes_received = bytes_received;
				if (!ec && bytes_received > 0)
				{
					ptr_se lpstruct = m_session.add(m_remoteport, -1);
					std::cout
						<< "[conv:"
						<< lpstruct->m_kcp->conv
						<< "][current:"
						<< lpstruct->m_kcp->current
						<< "][dead_link:"
						<< lpstruct->m_kcp->dead_link << "]" << std::endl;
					int linput = lpstruct->input(m_buff, bytes_received);
					if (linput >= 0)
					{
						while (true)
						{
							//从 buf中 提取真正数据，返回提取到的数据大小
							int lrecv = lpstruct->recv(m_buffrecv, 10240);
							if (lrecv == -3)
							{
								// ret == -3 m_buffrecv 的大小不够 
								close(lpstruct->m_session);
								break;
							}
							if (lrecv < 0)
							{
								break;
							}

							// 首先判断下是否kcp_cmd
							if (udp_cmd::cmd(lpstruct, m_buffrecv, lrecv))
							{
								std::cout << "kcp_cmd::cmd: " << std::string(m_buffrecv, lrecv) << std::endl;
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
						LogLocalError("[非kcp包:input < 0][%][%]"
							, m_remoteport.address().to_string()
							, m_remoteport.port()
						);
					}
					
					start();
				}
			});
	}

}