#pragma once

#include "type.h"
#include "asio_base.h"
#include "pack.h"
#include "nlog.h"
#include "threadtools.h"
#include "impl.h"
#include "ikcp.h"
#include "time_wheel.h"
#include "splite.h"
#include "ijson.h"
#include "ojson.h"

#include <map>
#include <memory>


namespace ngl
{

	using asio_udp = boost::asio::ip::udp;
	using asio_udp_endpoint = asio_udp::endpoint;

	class asio_kcp;

	extern time_wheel m_kcptimer;

	class nkcp
	{
		std::shared_ptr<ikcpcb> m_kcp;
		std::shared_ptr<std::shared_mutex> m_kcpmutex;
	public:
		nkcp()
			: m_kcp(nullptr)
			, m_kcpmutex(new std::shared_mutex())
		{}

		void create(IUINT32 asessionid, void* auser);

		int setmtu(int mtu)
		{
			monopoly_shared_lock(*m_kcpmutex);
			return ikcp_setmtu(m_kcp.get(), mtu);
		}

		typedef int (*output)(const char* buf, int len, struct IKCPCB* kcp, void* user);

		void setoutput(output afun)
		{
			monopoly_shared_lock(*m_kcpmutex);
			m_kcp->output = afun;
		}

		int nodelay(int nodelay, int interval, int resend, int nc)
		{
			monopoly_shared_lock(*m_kcpmutex);
			return ikcp_nodelay(m_kcp.get(), nodelay, interval, resend, nc);
		}

		int wndsize(int sndwnd, int rcvwnd)
		{
			monopoly_shared_lock(*m_kcpmutex);
			return ikcp_wndsize(m_kcp.get(), sndwnd, rcvwnd);
		}

		void update(IUINT32 current)
		{
			monopoly_shared_lock(*m_kcpmutex);
			ikcp_update(m_kcp.get(), current);
		}

		int input(const char* data, long size)
		{
			monopoly_shared_lock(*m_kcpmutex);
			return ikcp_input(m_kcp.get(), data, size);
		}

		int recv(char* buffer, int len)
		{
			monopoly_shared_lock(*m_kcpmutex);
			return ikcp_recv(m_kcp.get(), buffer, len);
		}

		int send(const char* buffer, int len)
		{
			monopoly_shared_lock(*m_kcpmutex);
			return ikcp_send(m_kcp.get(), buffer, len);
		}

		void flush()
		{
			monopoly_shared_lock(*m_kcpmutex);
			ikcp_flush(m_kcp.get());
		}

		void release()
		{
			monopoly_shared_lock(*m_kcpmutex);
			return ikcp_release(m_kcp.get());
		}
	};


	struct session_endpoint
	{
		i32_sessionid m_session;
		asio_udp_endpoint m_endpoint;
		std::string m_ip;
		i16_port m_port;
		asio_kcp* m_asiokcp;
		int64_t m_timerid;
		nkcp m_kcp;
		bool m_isconnect;		// 是否接收到kcp_cmd::ecmd_connect 或者ecmd_connect_ret
		int m_pingtm;			// 进行ping计时 
		int64_t m_pingtimerid;
		i64_actorid m_actorid;

		session_endpoint()
			: m_session(0)
			, m_port(0)
			, m_asiokcp(nullptr)
			, m_timerid(0)
			, m_isconnect(false)
			, m_pingtm(0)
			, m_pingtimerid(0)
			, m_actorid(-1)
		{}

		void create(IUINT32 asessionid, void* auser)
		{
			m_kcp.create(asessionid, auser);
		}

		void removetimer()
		{
			if (m_timerid != 0)
			{
				m_kcptimer.removetimer(m_timerid);
				std::cout << "removetimer" << std::endl;
			}
		}
	};

	int udp_output(const char* buf, int len, ikcpcb* kcp, void* user);

	class session_manage
	{
		std::map<i32_sessionid, session_endpoint> m_dataofsession;
		std::map<std::string, std::map<i16_port, session_endpoint*>> m_dataofendpoint;
		int32_t m_sessionid;

		std::shared_mutex m_mutex;
		asio_kcp* m_asiokcp;
	public:
		session_manage(asio_kcp* asiokcp) :
			m_sessionid(0),
			m_asiokcp(asiokcp)
		{}

		session_endpoint* add(const asio_udp_endpoint& aendpoint, i64_actorid aactorid)
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
			session_endpoint& ltemp = m_dataofsession[++m_sessionid];
			ltemp.m_session = m_sessionid;
			ltemp.m_endpoint = aendpoint;
			ltemp.m_ip = lip;
			ltemp.m_port = lport;
			ltemp.m_asiokcp = m_asiokcp;
			ltemp.m_actorid = aactorid;

			ltemp.create(m_sessionid, (void*)&ltemp);
			
			//设置kcp对象的回调函数
			ltemp.m_kcp.setoutput(udp_output);
			ltemp.m_kcp.nodelay(1, 10, 2, 1);
			ltemp.m_kcp.wndsize(128, 128);
			ltemp.m_kcp.setmtu(512);
			m_dataofendpoint[lip][lport] = &ltemp;

			int64_t lcreatems = time_wheel::getms();
			wheel_parm lparm
			{
				.m_ms = 10,
				.m_intervalms = [](int64_t) {return 10; } ,
				.m_count = 0x7fffffff,
				.m_fun = [&ltemp,lcreatems](wheel_node* anode)
				{
					ltemp.m_kcp.update((IUINT32)(time_wheel::getms() - lcreatems));
				}
			};
			m_kcptimer.addtimer(lparm);

			return &ltemp;
		}

		void erase(i32_sessionid asession)
		{
			monopoly_shared_lock(m_mutex);
			session_endpoint* lpstruct = _find(asession);
			if (lpstruct == nullptr)
				return;
			//std::string lip = lpstruct->m_endpoint.address().to_string();
			//i16_port lport = lpstruct->m_endpoint.port();
			auto& lmap = m_dataofendpoint[lpstruct->m_ip];
			lmap.erase(lpstruct->m_port);
			if (lmap.empty())
			{
				m_dataofendpoint.erase(lpstruct->m_ip);
			}
			m_dataofsession.erase(asession);
		}

		session_endpoint* _find(i32_sessionid asession)
		{
			auto itor = m_dataofsession.find(asession);
			if (itor == m_dataofsession.end())
				return nullptr;
			return &itor->second;
		}

		session_endpoint* find(i32_sessionid asession)
		{
			monopoly_shared_lock(m_mutex);
			auto itor = m_dataofsession.find(asession);
			if (itor == m_dataofsession.end())
				return nullptr;
			return &itor->second;
		}

		session_endpoint* find(const asio_udp_endpoint& aendpoint)
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
			return nullptr;
		}

		asio_udp_endpoint* find_endpoint(i32_sessionid asession)
		{
			monopoly_shared_lock(m_mutex);
			session_endpoint* lpstruct = _find(asession);
			if (lpstruct == nullptr)
				return nullptr;
			return &lpstruct->m_endpoint;
		}

	};

	class kcp_cmd
	{
	public:
		enum ecmd
		{
			ecmd_connect,				// 发起连接
			ecmd_connect_ret,			// 被发起连接者的返回
			ecmd_ping,					// 定时ping

			ecmd_minlen = sizeof("ecmd*") - 1,
		};
		using ecmd_callback = std::function<void(session_endpoint*, const std::string&)>;
		static std::map<ecmd, ecmd_callback> m_cmdfun;

		static bool cmd(session_endpoint* apstruct, const char* abuf, int32_t alen)
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

	class asio_kcp 
	{
		session_manage m_session;
		std::function<void(i32_session)> m_connectfun;
	public:
		asio_kcp(i16_port port);
	private:
		bpool m_pool;

		bool sempack(session_endpoint* apstruct, const char* abuff, int abufflen);
		void start();

	public:
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
		{
			session_endpoint* lpstruct = m_session.find(asessionid);
			if (lpstruct == nullptr)
				return false;
			
			send(lpstruct->m_endpoint, apack->m_buff, apack->m_len);
			return true;
		}

		bool sendpack(const asio_udp_endpoint& aendpoint, std::shared_ptr<pack>& apack)
		{
			send(aendpoint, apack->m_buff, apack->m_len);
			return true;
		}

		bool send(i32_sessionid asessionid, const char* buf, int len)
		{
			session_endpoint* lpstruct = m_session.find(asessionid);
			if (lpstruct == nullptr)
				return false;

			send(lpstruct->m_endpoint, buf, len);
			return true;
		}

		int send(const asio_udp_endpoint& aendpoint, const char* buf, int len)
		{
			//session_endpoint* lpstruct = m_session.add(aendpoint);
			//if (lpstruct == nullptr)
			//	return false;
			session_endpoint* lpstruct = m_session.find(aendpoint);
			if(lpstruct == nullptr)
				return false;
			int ret = lpstruct->m_kcp.send(buf, len);
			// 快速flush一次 以更快让客户端收到数据
			lpstruct->m_kcp.flush();
		}

		void connect(const std::string& akcpsess, i64_actorid aactorid, const std::string& aip, i16_port aport, const std::function<void(i32_session)>& afun)
		{
			ngl::asio_udp_endpoint lendpoint(boost::asio::ip::address::from_string(aip), aport);
			connect(akcpsess, aactorid, lendpoint, afun);
		}

		void connect(const std::string& akcpsess, i64_actorid aactorid, const asio_udp_endpoint& aendpoint, const std::function<void(i32_session)>& afun)
		{
			// #### 发起连接
			session_endpoint* lpstruct = m_session.add(aendpoint, aactorid);
			ijson ltempjson;
			ltempjson << std::make_pair("actorid", aactorid);
			ltempjson << std::make_pair("session", akcpsess);
			ltempjson.set_nonformatstr(true);
			std::string lparm;
			ltempjson >> lparm;
			kcp_cmd::sendcmd(this, lpstruct->m_session, kcp_cmd::ecmd_connect, lparm);
			m_connectfun = afun;
		}

		i64_actorid find_actorid(i32_session asession)
		{
			session_endpoint* lpstruct = m_session.find(asession);
			if (lpstruct == nullptr)
				return -1;
			return lpstruct->m_actorid;
		}

		void close(i32_session asession)
		{
			m_session.erase(asession);
		}

		int sendbuff(i32_session asession, const char* buf, int len)
		{
			session_endpoint* lpstruct = m_session.find(asession);
			m_socket.async_send_to(boost::asio::buffer(buf, len), lpstruct->m_endpoint, [](const boost::system::error_code& ec, std::size_t bytes_received)
				{
					if (ec)
					{
						std::cout << ec.what() << std::endl;
					}
				});
			return 0;
		}

		int sendbuff(asio_udp_endpoint& aendpoint, const char* buf, int len)
		{
			m_socket.async_send_to(boost::asio::buffer(buf, len), aendpoint, [](const boost::system::error_code& ec, std::size_t bytes_received)
				{
					if (ec)
					{
						std::cout << ec.what() << std::endl;
					}
				});
			return 0;
		}
	private:

		boost::asio::io_context m_context;
		asio_udp::socket m_socket;
		asio_udp_endpoint m_remoteport;
		char m_buff[1500];
		char m_buffrecv[10240];
	};


	
}