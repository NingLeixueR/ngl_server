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
		std::cout << "send###" << buf << std::endl;
		session_endpoint* lpstruct = (session_endpoint*)user;
		lpstruct->m_asiokcp->sendbuff(lpstruct->m_endpoint, buf, len);
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

	std::map<kcp_cmd::ecmd, kcp_cmd::ecmd_callback> kcp_cmd::m_cmdfun;

	void kcp_cmd::sendcmd(asio_kcp* akcp, i32_sessionid asession, kcp_cmd::ecmd acmd, const std::string& ajson)
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

		auto lcallfun = [lpsession, this](session_endpoint* apstruct)->bool
		{
				return true;
				if (nconfig::m_nodetype == ngl::ROBOT)
					return true;
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
						session_endpoint* lpstruct = lpsession->find(session);
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
				return true;
		};

		kcp_cmd::register_fun(kcp_cmd::ecmd_connect, [this, lcallfun](session_endpoint* apstruct, const std::string& ajson)
			{
				apstruct->m_isconnect = true;
				apstruct->m_pingtm = localtime::gettime();
				ojson ltempjson(ajson.c_str());
				std::pair<const char*, i64_actorid> lactorpair("actorid", 0);
				if ((ltempjson >> lactorpair) == false)
					return;
				apstruct->m_actorid = lactorpair.second;

				if (lcallfun(apstruct))
					kcp_cmd::sendcmd(this, apstruct->m_session, kcp_cmd::ecmd_connect_ret, "");
			});

		kcp_cmd::register_fun(kcp_cmd::ecmd_connect_ret, [lpsession, this, lcallfun](session_endpoint* apstruct, const std::string& ajson)
			{
				apstruct->m_isconnect = true;
				apstruct->m_pingtm = localtime::gettime();

				lcallfun(apstruct);
				m_connectfun(apstruct->m_session);
			});

		kcp_cmd::register_fun(kcp_cmd::ecmd_ping, [lpsession, this, lcallfun](session_endpoint* apstruct, const std::string& ajson)
			{
				apstruct->m_pingtm = localtime::gettime();
			});

		new thread([this]()
			{
				m_context.run();
			});
		start();
	}

	bool asio_kcp::sempack(session_endpoint* apstruct, const char* abuff, int abufflen)
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
				if (!ec && bytes_received > 0)
				{
					session_endpoint* lpstruct = m_session.add(m_remoteport, -1);
					int ret = lpstruct->m_kcp.input(m_buff, bytes_received);
					if (ret >= 0)
					{
						while (true)
						{

							//从 buf中 提取真正数据，返回提取到的数据大小
							int ret = lpstruct->m_kcp.recv(m_buffrecv, 10240);
							if (ret == -3)
							{
								// ret == -3 m_buffrecv 的大小不够 
								close(lpstruct->m_session);
								break;
							}
							if (ret < 0)
							{ 	
								break;
							}

							// 首先判断下是否kcp_cmd
							if (kcp_cmd::cmd(lpstruct, m_buffrecv, bytes_received))
							{
								std::cout << "kcp_cmd::cmd: " << std::string(m_buffrecv, ret) << std::endl;
								break;
							}

							if (lpstruct->m_isconnect == false)
							{
								break;
							}

							if (sempack(lpstruct, m_buffrecv, ret) == false)
							{
								close(lpstruct->m_session);
								break;
							}
							
							std::cout << "Received message: " << std::string(m_buffrecv, ret) << std::endl;
						}
					}
					else
					{
						std::cout << "input < 0" << std::endl;
					}

					start();
				}
			});
	}

}