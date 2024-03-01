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

	int udp_output(const char* buf, int len, ikcpcb* kcp, void* user);

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

		session_endpoint()
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

		~session_endpoint()
		{
			removetimer();
			release();
		}

		static std::string ip(session_endpoint* ap)
		{
			return ap->m_endpoint.address().to_string();
		}

		static i16_port port(session_endpoint* ap)
		{
			return ap->m_endpoint.port();
		}

		void create(int32_t aconv, IUINT32 asessionid, void* auser)
		{
			m_kcp = ikcp_create(aconv, auser);
		}

		void removetimer()
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

		int setmtu(int mtu)
		{
			return ikcp_setmtu(m_kcp, mtu);
		}

		void setoutput(output afun)
		{
			m_kcp->output = afun;
		}

		int nodelay(int nodelay, int interval, int resend, int nc)
		{
			return ikcp_nodelay(m_kcp, nodelay, interval, resend, nc);
		}

		int wndsize(int sndwnd, int rcvwnd)
		{
			return ikcp_wndsize(m_kcp, sndwnd, rcvwnd);
		}

		void update(IUINT32 current)
		{
			ikcp_update(m_kcp, current);
		}

		int input(const char* data, long size)
		{
			return ikcp_input(m_kcp, data, size);
		}

		int recv(char* buffer, int len)
		{
			return ikcp_recv(m_kcp, buffer, len);
		}

		int send(const char* buffer, int len)
		{
			return ikcp_send(m_kcp, buffer, len);
		}

		void flush()
		{
			ikcp_flush(m_kcp);
		}

		void release()
		{
			return ikcp_release(m_kcp);
		}
	};

	using ptr_se = std::shared_ptr<session_endpoint>;

	class session_manage
	{
		std::map<i32_sessionid, ptr_se>						m_dataofsession;
		std::map<std::string, std::map<i16_port, ptr_se>>	m_dataofendpoint;
		int32_t												m_sessionid;
		std::shared_mutex									m_mutex;
		asio_kcp*											m_asiokcp;
	public:
		session_manage(asio_kcp* asiokcp) :
			m_sessionid(0),
			m_asiokcp(asiokcp)
		{}

		ptr_se add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactorid)
		{
			std::string lip = aendpoint.address().to_string();
			i16_port lport = aendpoint.port();
			monopoly_shared_lock(m_mutex);
			auto itor = m_dataofendpoint.find(lip);
			if (itor != m_dataofendpoint.end())
			{
				auto itorport = itor->second.find(lport);
				if (itorport != itor->second.end())
				{
					return itorport->second;
				}
			}
			if (aconv <= 0)
				return nullptr;
			ptr_se ltemp(new session_endpoint());
			m_dataofsession[++m_sessionid] = ltemp;
			ltemp->m_session = m_sessionid;
			ltemp->m_endpoint = aendpoint;
			ltemp->m_ip = lip;
			ltemp->m_port = lport;
			ltemp->m_asiokcp = m_asiokcp;
			ltemp->m_actorid = aactorid;
			ltemp->create(aconv, m_sessionid, (void*)ltemp.get());

			//设置kcp对象的回调函数
			ltemp->setoutput(udp_output);
			ltemp->nodelay(1, 10, 2, 1);
			ltemp->wndsize(128, 128);
			ltemp->setmtu(512);
			m_dataofendpoint[lip][lport] = ltemp;

			int64_t lcreatems = time_wheel::getms();
			wheel_parm lparm
			{
				.m_ms = 10,
				.m_intervalms = [](int64_t) {return 10; } ,
				.m_count = 0x7fffffff,
				.m_fun = [ltemp,lcreatems](wheel_node* anode)
				{
					ltemp->update((IUINT32)(time_wheel::getms() - lcreatems));
				}
			};
			m_kcptimer.addtimer(lparm);
			return ltemp;
		}

		ptr_se reset_add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactorid)
		{
			erase(aendpoint);
			return add(aconv, aendpoint, aactorid);
		}

		ptr_se erase(const asio_udp_endpoint& aendpoint)
		{
			std::string lip = aendpoint.address().to_string();
			i16_port lport = aendpoint.port();
			monopoly_shared_lock(m_mutex);
			ptr_se lpstruct = _find(aendpoint);
			if (lpstruct == nullptr)
				return nullptr;
			auto& lmap = m_dataofendpoint[lpstruct->m_ip];
			lmap.erase(lpstruct->m_port);
			if (lmap.empty())
			{
				m_dataofendpoint.erase(lpstruct->m_ip);
			}
			m_dataofsession.erase(lpstruct->m_session);
		}

		void erase(i32_sessionid asession)
		{
			monopoly_shared_lock(m_mutex);
			ptr_se lpstruct = _find(asession);
			if (lpstruct == nullptr)
				return;
			auto& lmap = m_dataofendpoint[lpstruct->m_ip];
			lmap.erase(lpstruct->m_port);
			if (lmap.empty())
			{
				m_dataofendpoint.erase(lpstruct->m_ip);
			}
			m_dataofsession.erase(asession);
		}
	private:
		ptr_se _find(i32_sessionid asession)
		{
			auto itor = m_dataofsession.find(asession);
			if (itor == m_dataofsession.end())
				return nullptr;
			return itor->second;
		}

		ptr_se _find(const asio_udp_endpoint& aendpoint)
		{
			std::string lip = aendpoint.address().to_string();
			i16_port lport = aendpoint.port();
			auto itor = m_dataofendpoint.find(lip);
			if (itor != m_dataofendpoint.end())
			{
				auto itorport = itor->second.find(lport);
				if (itorport != itor->second.end())
				{
					return itorport->second;
				}
			}
			return nullptr;
		}
	public:
		ptr_se find(i32_sessionid asession)
		{
			monopoly_shared_lock(m_mutex);
			auto itor = m_dataofsession.find(asession);
			if (itor == m_dataofsession.end())
				return nullptr;
			return itor->second;
		}

		ptr_se find(const asio_udp_endpoint& aendpoint)
		{
			monopoly_shared_lock(m_mutex);
			return _find(aendpoint);
		}

		asio_udp_endpoint* find_endpoint(i32_sessionid asession)
		{
			monopoly_shared_lock(m_mutex);
			ptr_se lpstruct = _find(asession);
			if (lpstruct == nullptr)
				return nullptr;
			return &lpstruct->m_endpoint;
		}
	};

	class udp_cmd
	{
	public:
		enum ecmd
		{
			ecmd_connect,				// 发起连接
			ecmd_connect_ret,			// 被发起连接者的返回
			ecmd_ping,					// 定时ping
			ecmd_close,					// 主动断开连接
			ecmd_close_ret,             // 主动断开连接的返回
			ecmd_minlen = sizeof("ecmd*") - 1,
		};
		using ecmd_callback = std::function<void(ptr_se&, const std::string&)>;
		static std::map<ecmd, ecmd_callback> m_cmdfun;

		static bool cmd(ptr_se& apstruct, const char* abuf, int32_t alen)
		{
			if (alen < ecmd_minlen)
				return false;
			if (memcmp(abuf, "ecmd*", ecmd_minlen) != 0)
				return false;
			try
			{
				std::string lecmd;
				int32_t lnum = 0;
				std::string ljson;
				splite::division(abuf, "*", lecmd, lnum, ljson);
				//ecmd lnum = (ecmd)boost::lexical_cast<int32_t>(&abuf[ecmd_minlen]);
				auto itor = m_cmdfun.find((ecmd)lnum);
				if (itor != m_cmdfun.end())
					itor->second(apstruct, ljson);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		static void register_fun(ecmd anum, const ecmd_callback& afun)
		{
			m_cmdfun[anum] = afun;
		}

		static void sendcmd(asio_kcp* akcp, i32_sessionid asession, ecmd acmd, const std::string& ajson);
	};

	struct asio_kcp::impl_asio_kcp
	{
		session_manage						m_session;
		std::function<void(i32_session)>	m_connectfun;
		bpool								m_pool;
		boost::asio::io_context				m_context;
		asio_udp::socket					m_socket;
		asio_udp_endpoint					m_remoteport;
		char								m_buff[1500];
		std::size_t							m_bytes_received;
		char								m_buffrecv[10240];
		asio_kcp*							m_kcp;
	private:
		//## [udp_cmd::ecmd_connect]		调用aconnect为true
		//## [udp_cmd::ecmd_connect_ret]	调用aconnect为true
		inline bool function_econnect(ptr_se& apstruct, bool aconnect)
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
			if (aconnect)
			{
				wheel_parm lparm
				{
					.m_ms = ms * 1000,
					.m_intervalms = [ms](int64_t) {return ms * 1000; } ,
					.m_count = 0x7fffffff,
					.m_fun = [session,ms,intervalms, this](wheel_node* anode)
					{
						ptr_se lpstruct = m_session.find(session);
						if (lpstruct == nullptr)
						{
							m_kcptimer.removetimer(anode->m_timerid);
							return;
						}
						int32_t lnow = localtime::gettime();
						if (lnow - lpstruct->m_pingtm > intervalms)
						{
							m_kcp->close(lpstruct->m_session);
							m_kcptimer.removetimer(anode->m_timerid);
						}
					}
				};
				apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
			}
			else
			{
				wheel_parm lparm
				{
					.m_ms = ms * 1000,
					.m_intervalms = [ms](int64_t) {return ms * 1000; } ,
					.m_count = 0x7fffffff,
					.m_fun = [session, ms, intervalms, this](wheel_node* anode)
					{
						ptr_se lpstruct = m_session.find(session);
						if (lpstruct == nullptr)
						{
							m_kcptimer.removetimer(anode->m_timerid);
							return;
						}
						udp_cmd::sendcmd(m_kcp, session, udp_cmd::ecmd_ping, "");
					}
				};
				apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
			}
			return true;
		}
		//## udp_cmd::ecmd_connect
		inline void function_ecmd_connect()
		{
			udp_cmd::register_fun(udp_cmd::ecmd_connect, [this](ptr_se& apstruct, const std::string& ajson)
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
					if (ukcp::check_session(lactorpair.second, lsessionpair.second) == false)
						return;

					apstruct->m_actorid = lactorpair.second;

					std::cout << "kcp connect : "
						<< session_endpoint::ip(apstruct.get())
						<< "@"
						<< session_endpoint::port(apstruct.get())
						<< std::endl;

					if (function_econnect(apstruct, true))
						udp_cmd::sendcmd(m_kcp, apstruct->m_session, udp_cmd::ecmd_connect_ret, "");
				});
		}
		//## udp_cmd::ecmd_connect_ret
		inline void function_ecmd_connect_ret()
		{
			// 除了robot 其他服务器均不允许定时ping 
			//if (nconfig::m_nodetype != ngl::ROBOT)
			//	return;
			udp_cmd::register_fun(udp_cmd::ecmd_connect_ret, [this](ptr_se& apstruct, const std::string& ajson)
				{
					apstruct->m_isconnect = true;
					apstruct->m_pingtm = localtime::gettime();
					// 定时发送cmd:ping
					function_econnect(apstruct, false);
					m_connectfun(apstruct->m_session);
				});
		}
		//## udp_cmd::ecmd_ping
		inline void function_ecmd_ping()
		{
			udp_cmd::register_fun(udp_cmd::ecmd_ping, [this](ptr_se& apstruct, const std::string& ajson)
				{
					apstruct->m_pingtm = localtime::gettime();
				});
		}
		//## udp_cmd::ecmd_close
		inline void function_ecmd_close()
		{
			udp_cmd::register_fun(udp_cmd::ecmd_close, [this](ptr_se& apstruct, const std::string& ajson)
				{
					udp_cmd::sendcmd(m_kcp, apstruct->m_session, udp_cmd::ecmd_close_ret, "");
					int lession = apstruct->m_session;
					wheel_parm lparm
					{
						.m_ms = 1000,
						.m_intervalms = [](int64_t) {return 1000; } ,
						.m_count = 0x7fffffff,
						.m_fun = [this,lession](wheel_node* anode)
						{
							close(lession);
						}
					};
					apstruct->m_pingtimerid = m_kcptimer.addtimer(lparm);
				});
		}
	public:
		inline impl_asio_kcp(asio_kcp* akcp, i16_port port)
			: m_context()
			, m_socket(m_context, asio_udp_endpoint(asio_udp::v4(), port))
			, m_session(akcp)
			, m_kcp(akcp)
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

		inline bool sempack(ptr_se& apstruct, const char* abuff, int abufflen)
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

		inline void start()
		{
			m_socket.async_receive_from(boost::asio::buffer(m_buff, 1500), m_remoteport,
				[this](const boost::system::error_code& ec, std::size_t bytes_received)
				{
					m_bytes_received = bytes_received;
					if (!ec && bytes_received > 0)
					{
						ptr_se lpstruct = m_session.find(m_remoteport);
						if (lpstruct != nullptr)
						{
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
						}
						else
						{
							//NFC = not find connect
							sendu(m_remoteport, "NFC", sizeof("NFC"));
						}
						start();
					}
				});
		};

		// ## 发送原始udp包
		inline bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len)
		{
			m_socket.async_send_to(boost::asio::buffer(buf, len), aendpoint, [](const boost::system::error_code& ec, std::size_t bytes_received)
				{
					if (ec)
						std::cout << ec.what() << std::endl;
				});
			return true;
		}
		
		// ## 通过kcp发送pack
		inline bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
		{
			ptr_se lpstruct = m_session.find(asessionid);
			if (lpstruct == nullptr)
				return false;

			send(lpstruct->m_endpoint, apack->m_buff, apack->m_len);
			return true;
		}
		
		// ## 通过kcp发送pack
		inline bool sendpack(const asio_udp_endpoint& aendpoint, std::shared_ptr<pack>& apack)
		{
			send(aendpoint, apack->m_buff, apack->m_len);
			return true;
		}
		
		inline bool send(i32_sessionid asessionid, const char* buf, int len)
		{
			ptr_se lpstruct = m_session.find(asessionid);
			if (lpstruct == nullptr)
				return false;

			send(lpstruct->m_endpoint, buf, len);
			return true;
		}

		inline int send(const asio_udp_endpoint& aendpoint, const char* buf, int len)
		{
			ptr_se lpstruct = m_session.find(aendpoint);
			if (lpstruct == nullptr)
				return false;
			int ret = lpstruct->send(buf, len);
			// 快速flush一次 以更快让客户端收到数据
			lpstruct->flush();
		}

		// ## kcp发送回调会调用的方法
		inline int sendbuff(i32_session asession, const char* buf, int len)
		{
			ptr_se lpstruct = m_session.find(asession);
			m_socket.async_send_to(boost::asio::buffer(buf, len), lpstruct->m_endpoint, [](const boost::system::error_code& ec, std::size_t bytes_received)
				{
					if (ec)
						LogLocalError("impl_asio_kcp::sendbuff error [%]", ec.what());
				});
			return 0;
		}

		inline int sendbuff(asio_udp_endpoint& aendpoint, const char* buf, int len)
		{
			std::cout << aendpoint.address().to_string() << ":" << aendpoint.port() << std::endl;
			m_socket.async_send_to(boost::asio::buffer(buf, len), aendpoint, [](const boost::system::error_code& ec, std::size_t bytes_received)
				{
					if (ec)
						LogLocalError("impl_asio_kcp::sendbuff error [%]", ec.what());
				});
			return 0;
		}

		inline void connect(int32_t aconv
			, const std::string& akcpsess
			, i64_actorid aactorid
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		)
		{
			ngl::asio_udp_endpoint lendpoint(boost::asio::ip::address::from_string(aip), aport);
			connect(aconv, akcpsess, aactorid, lendpoint, afun);
		}

		inline void connect(int32_t aconv
			, const std::string& akcpsess
			, i64_actorid aactorid
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		)
		{
			// #### 发起连接
			ptr_se lpstruct = m_session.add(aconv, aendpoint, aactorid);
			ijson ltempjson;
			ltempjson << std::make_pair("actorid", aactorid);
			ltempjson << std::make_pair("session", akcpsess);
			ltempjson.set_nonformatstr(true);
			std::string lparm;
			ltempjson >> lparm;
			udp_cmd::sendcmd(m_kcp, lpstruct->m_session, udp_cmd::ecmd_connect, lparm);
			m_connectfun = afun;
		}

		i64_actorid find_actorid(i32_session asession)
		{
			ptr_se lpstruct = m_session.find(asession);
			if (lpstruct == nullptr)
				return -1;
			return lpstruct->m_actorid;
		}

		void close(i32_session asession)
		{
			m_session.erase(asession);
		}

		void reset_add(int32_t aconv, const std::string& aip, i16_port aport)
		{
			ngl::asio_udp_endpoint lendpoint(boost::asio::ip::address::from_string(aip), aport);
			m_session.reset_add(aconv, lendpoint, -1);
		}
	};

	int udp_output(const char* buf, int len, ikcpcb* kcp, void* user)
	{
		session_endpoint* lpstruct = (session_endpoint*)user;
		lpstruct->m_asiokcp->get_impl()->sendbuff(lpstruct->m_endpoint, buf, len);
		return len;
	}

	std::map<udp_cmd::ecmd, udp_cmd::ecmd_callback> udp_cmd::m_cmdfun;

	void udp_cmd::sendcmd(asio_kcp* akcp, i32_sessionid asession, udp_cmd::ecmd acmd, const std::string& ajson)
	{
		std::stringstream lstream;
		lstream << "ecmd*" << (int)acmd << "*" << ajson;
		std::string lstr = lstream.str();
		akcp->get_impl()->send(asession, lstr.c_str(), lstr.size()+1);
	}

	asio_kcp::asio_kcp(i16_port port)
	{
		m_impl_asio_kcp.make_unique(this, port);
	}

	asio_kcp::~asio_kcp() {}

	bool asio_kcp::sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len)
	{
		return m_impl_asio_kcp()->sendu(aendpoint, buf, len);
	}

	bool asio_kcp::sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
	{
		return m_impl_asio_kcp()->sendpack(asessionid, apack);
	}

	bool asio_kcp::sendpack(const asio_udp_endpoint& aendpoint, std::shared_ptr<pack>& apack)
	{
		return m_impl_asio_kcp()->sendpack(aendpoint, apack);
	}

	void asio_kcp::connect(int32_t aconv
		, const std::string& akcpsess
		, i64_actorid aactorid
		, const std::string& aip
		, i16_port aport
		, const std::function<void(i32_session)>& afun
	)
	{
		return m_impl_asio_kcp()->connect(aconv, akcpsess, aactorid, aip, aport, afun);
	}

	void asio_kcp::connect(int32_t aconv
		, const std::string& akcpsess
		, i64_actorid aactorid
		, const asio_udp_endpoint& aendpoint
		, const std::function<void(i32_session)>& afun
	)
	{
		return m_impl_asio_kcp()->connect(aconv, akcpsess, aactorid, aendpoint, afun);
	}

	i64_actorid asio_kcp::find_actorid(i32_session asession)
	{
		return m_impl_asio_kcp()->find_actorid(asession);
	}

	void asio_kcp::close(i32_session asession)
	{
		m_impl_asio_kcp()->close(asession);
	}

	void asio_kcp::reset_add(int32_t aconv, const std::string& aip, i16_port aport)
	{
		m_impl_asio_kcp()->reset_add(aconv, aip, aport);
	}

	asio_kcp::impl_asio_kcp* asio_kcp::get_impl()
	{
		return m_impl_asio_kcp().get();
	}
}