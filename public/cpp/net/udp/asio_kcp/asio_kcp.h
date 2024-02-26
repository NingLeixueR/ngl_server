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



	struct session_endpoint
	{
		i32_sessionid m_session;
		asio_udp_endpoint m_endpoint;
		std::string m_ip;
		i16_port m_port;
		asio_kcp* m_asiokcp;
		int64_t m_timerid;
		ikcpcb* m_kcp;
		bool m_isconnect;		// 是否接收到kcp_cmd::ecmd_connect 或者ecmd_connect_ret
		int m_pingtm;			// 进行ping计时 
		int64_t m_pingtimerid;
		i64_actorid m_actorid;
		std::shared_mutex m_mutex;

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

		~session_endpoint()
		{
			removetimer();
			release();
		}

		void create(IUINT32 asessionid, void* auser)
		{
			//monopoly_shared_lock(m_mutex);
			m_kcp = ikcp_create(1, auser);
		}

		void removetimer()
		{
			//monopoly_shared_lock(m_mutex);
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
			monopoly_shared_lock(m_mutex);
			return ikcp_setmtu(m_kcp, mtu);
		}

		typedef int (*output)(const char* buf, int len, struct IKCPCB* kcp, void* user);

		void setoutput(output afun)
		{
			monopoly_shared_lock(m_mutex);
			m_kcp->output = afun;
		}

		int nodelay(int nodelay, int interval, int resend, int nc)
		{
			monopoly_shared_lock(m_mutex);
			return ikcp_nodelay(m_kcp, nodelay, interval, resend, nc);
		}

		int wndsize(int sndwnd, int rcvwnd)
		{
			monopoly_shared_lock(m_mutex);
			return ikcp_wndsize(m_kcp, sndwnd, rcvwnd);
		}

		void update(IUINT32 current)
		{
			monopoly_shared_lock(m_mutex);
			ikcp_update(m_kcp, current);
		}

		int input(const char* data, long size)
		{
			monopoly_shared_lock(m_mutex);
			return ikcp_input(m_kcp, data, size);
		}

		int recv(char* buffer, int len)
		{
			monopoly_shared_lock(m_mutex);
			return ikcp_recv(m_kcp, buffer, len);
		}

		int send(const char* buffer, int len)
		{
			monopoly_shared_lock(m_mutex);
			return ikcp_send(m_kcp, buffer, len);
		}

		void flush()
		{
			monopoly_shared_lock(m_mutex);
			ikcp_flush(m_kcp);
		}

		void release()
		{
			monopoly_shared_lock(m_mutex);
			return ikcp_release(m_kcp);
		}
	};

	int udp_output(const char* buf, int len, ikcpcb* kcp, void* user);
	
	using ptr_se = std::shared_ptr<session_endpoint>;

	class session_manage
	{
		// 
		
		std::map<i32_sessionid, ptr_se> m_dataofsession;
		std::map<std::string, std::map<i16_port, ptr_se>> m_dataofendpoint;
		int32_t m_sessionid;

		std::shared_mutex m_mutex;
		asio_kcp* m_asiokcp;
	public:
		session_manage(asio_kcp* asiokcp) :
			m_sessionid(0),
			m_asiokcp(asiokcp)
		{}

		ptr_se add(const asio_udp_endpoint& aendpoint, i64_actorid aactorid)
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
			ptr_se ltemp(new session_endpoint());
			m_dataofsession[++m_sessionid] = ltemp;
			ltemp->m_session = m_sessionid;
			ltemp->m_endpoint = aendpoint;
			ltemp->m_ip = lip;
			ltemp->m_port = lport;
			ltemp->m_asiokcp = m_asiokcp;
			ltemp->m_actorid = aactorid;
			ltemp->create(m_sessionid, (void*)ltemp.get());
			
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

		ptr_se reset_add(const asio_udp_endpoint& aendpoint, i64_actorid aactorid)
		{
			erase(aendpoint);
			return add(aendpoint, aactorid);
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
			ecmd_close_ret,				// 主动断开连接的返回

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

	class asio_kcp 
	{
		session_manage m_session;
		std::function<void(i32_session)> m_connectfun;
	public:
		asio_kcp(i16_port port);
	private:
		bpool m_pool;

		bool sempack(ptr_se& apstruct, const char* abuff, int abufflen);
		void start();

	public:
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
		{
			ptr_se lpstruct = m_session.find(asessionid);
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
			ptr_se lpstruct = m_session.find(asessionid);
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
			ptr_se lpstruct = m_session.find(aendpoint);
			if(lpstruct == nullptr)
				return false;
			int ret = lpstruct->send(buf, len);
			// 快速flush一次 以更快让客户端收到数据
			lpstruct->flush();
		}

		void connect(const std::string& akcpsess, i64_actorid aactorid, const std::string& aip, i16_port aport, const std::function<void(i32_session)>& afun)
		{
			ngl::asio_udp_endpoint lendpoint(boost::asio::ip::address::from_string(aip), aport);
			connect(akcpsess, aactorid, lendpoint, afun);
		}

		void connect(const std::string& akcpsess, i64_actorid aactorid, const asio_udp_endpoint& aendpoint, const std::function<void(i32_session)>& afun)
		{
			// #### 发起连接
			ptr_se lpstruct = m_session.add(aendpoint, aactorid);
			ijson ltempjson;
			ltempjson << std::make_pair("actorid", aactorid);
			ltempjson << std::make_pair("session", akcpsess);
			ltempjson.set_nonformatstr(true);
			std::string lparm;
			ltempjson >> lparm;
			udp_cmd::sendcmd(this, lpstruct->m_session, udp_cmd::ecmd_connect, lparm);
			m_connectfun = afun;
		}

		i64_actorid find_actorid(i32_session asession)
		{
			ptr_se lpstruct = m_session.find(asession);
			if (lpstruct == nullptr)
				return -1;
			return lpstruct->m_actorid;
		}
		//  bool aactive 主动关闭
		void close(i32_session asession, bool aactive = false)
		{
			auto apstruct = m_session.find(asession);
			if (aactive)
			{
				udp_cmd::sendcmd(this, apstruct->m_session, udp_cmd::ecmd_close, "");
			}
			else
			{
				udp_cmd::sendcmd(this, apstruct->m_session, udp_cmd::ecmd_close_ret, "");
			}
			m_session.erase(asession);
		}

		int sendbuff(i32_session asession, const char* buf, int len)
		{
			ptr_se lpstruct = m_session.find(asession);
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
			std::cout << aendpoint.address().to_string() << ":" << aendpoint.port() << std::endl;
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