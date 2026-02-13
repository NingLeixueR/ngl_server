/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "sysconfig.h"
#include "asio_kcp.h"
#include "protocol.h"
#include "ukcp.h"
#include "cmd.h"

#define USE_WHEEL_TIMER

namespace ngl
{
	//## udp_cmd::ecmd_connect
	void asio_kcp::func_ecmd_connect()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_connect, [](asio_kcp* ap, ptr_se& apstruct, const std::string& ajson)
			{
				apstruct->m_isconnect = true;
				ncjson ltempjson(ajson.c_str());

				i64_actorid lclient = nguid::make();
				i64_actorid lserver = nguid::make();
				std::string lsession;
				if (!njson::pop(ltempjson.json(), { "actoridclient","actoridserver","session" }, lclient, lserver, lsession))
				{
					return;
				}
				if (ukcp::session_check(lserver, lclient, lsession) == false)
				{
					return;
				}
				
				apstruct->m_client = lclient;
				apstruct->m_server = lserver;

				log_error()->print("kcp connect : {}@{}", kcp_endpoint::ip(apstruct.get()), kcp_endpoint::port(apstruct.get()));

				udp_cmd::sendcmd(ap, apstruct->m_session, udp_cmd::ecmd_connect_ret, "{}");
			}
		);
	}

	//## udp_cmd::ecmd_connect_ret
	void asio_kcp::func_ecmd_connect_ret()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_connect_ret, [](asio_kcp* ap, ptr_se& apstruct, const std::string& ajson)
			{
				apstruct->m_isconnect = true;
				ap->m_connectfun(apstruct->m_session);
			}
		);
	}

	//## udp_cmd::ecmd_close
	void asio_kcp::func_ecmd_close()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_close, [](asio_kcp* ap, ptr_se& apstruct, const std::string&)
			{
				udp_cmd::sendcmd(ap, apstruct->m_session, udp_cmd::ecmd_close_ret, "");
				int lession = apstruct->m_session;
				wheel_parm lparm
				{
					.m_ms = e_close_intervalms,
					.m_intervalms = [](int64_t) {return e_close_intervalms; },
					.m_count = 0x7fffffff,
					.m_fun = [ap,lession](const wheel_node*)
					{
						ap->close(lession);
					}
				};
				m_kcptimer.addtimer(lparm);
			}
		);
	}

	asio_kcp::asio_kcp(i16_port port)
		: m_context()
		, m_port(port)
		, m_socket(m_context, asio_udp_endpoint(asio_udp::v4(), port))
		, m_session(this)
		, m_thread([this](){m_context.run();})
	{
		func_ecmd_connect();
		func_ecmd_connect_ret();
		func_ecmd_close();

		start();
	}

	bool asio_kcp::sempack(const ptr_se& apstruct, const char* abuff, int abufflen)
	{
		if (!m_rate.add(apstruct->m_session))
		{
			return false;
		}
		// 获取包头
		std::shared_ptr<pack> lpack = pack::make_pack(&m_pool, 0);
		lpack->m_protocol = ENET_KCP;
		lpack->m_id = apstruct->m_session;
		if (EPH_HEAD_SUCCESS != lpack->m_head.push(abuff, abufflen))
		{
			return false;
		}
		int len = lpack->m_head.getvalue(EPH_BYTES);
		if (len < 0 || len != abufflen)
		{
			return false;
		}
		lpack->malloc(len);
		memcpy(lpack->m_buff, abuff, len);
		
		i64_actorid lactorid;
		if (nconfig.nodetype() != ROBOT)
		{
			lpack->m_head.set_actor(apstruct->m_server, apstruct->m_client);
		}
		
		lpack->m_pos = len;
		if (localtime::gettime() < lpack->m_head.getvalue(EPH_TIME) + sysconfig::net_timeout())
		{
			protocol::push(lpack);
		}
		else
		{
			log_error()->print("time[{} < {} + {} ]", localtime::gettime(), lpack->m_head.getvalue(EPH_TIME), sysconfig::net_timeout());
		}

		return true;
	}

	void asio_kcp::start()
	{
		m_socket.async_receive_from(asio::buffer(m_buff, e_buff_byte), m_remoteport,
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
									//从 buf中 提取真正数据，返回提取到的数据大小
									int lrecv = lpstruct->recv(m_buffrecv, e_buffrecv_byte);
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

	// ## 发送原始udp包
	bool asio_kcp::sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len)
	{
		m_socket.async_send_to(asio::buffer(buf, len), aendpoint, [](const std::error_code& ec, std::size_t bytes_received)
			{
				if (ec)
				{
					log_error()->print("sendu err [{}]", ec.message());
				}
			}
		);
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
						.m_ms = e_waitrecv_intervalms,
						.m_intervalms = [](int64_t) {return e_waitrecv_intervalms; } ,
						.m_count = 1,
						.m_fun = [this, aendpoint, buf, len, afun](const wheel_node*)
						{
							sendu_waitrecv(aendpoint, buf, len, afun);
						}
					};
					m_kcptimer.addtimer(lparm);
				}
			}
		);
		return true;
	}

	// ## 通过kcp发送pack
	bool asio_kcp::send_server(i32_sessionid asessionid, const std::shared_ptr<pack>& apack)
	{
		ptr_se lpstruct = m_session.find(asessionid);
		if (lpstruct == nullptr)
		{
			return false;
		}

		send(lpstruct->m_endpoint, apack->m_buff, apack->m_len);
		return true;
	}

	bool asio_kcp::send_server(const std::shared_ptr<pack>& apack)
	{
		m_session.foreach([this, &apack](ptr_se& aptr)
			{
				send(aptr->m_endpoint, apack->m_buff, apack->m_len);
			}
		);
		return true;
	}

	bool asio_kcp::sendpackbyarea(i16_area aarea, const std::shared_ptr<pack>& apack)
	{
		m_session.foreach([this, &apack, aarea](ptr_se& aptr)
			{
				send(aptr->m_endpoint, apack->m_buff, apack->m_len);
			}
		);
		return true;
	}

	// ## 通过kcp发送pack
	bool asio_kcp::send_server(const asio_udp_endpoint& aendpoint, const std::shared_ptr<pack>& apack)
	{
		send(aendpoint, apack->m_buff, apack->m_len);
		return true;
	}

	bool asio_kcp::sendpackbyactorid(i64_actorid aactorid, const std::shared_ptr<pack>& apack)
	{
		ptr_se lpstruct = m_session.findbyactorid(aactorid);
		if (lpstruct == nullptr)
		{
			return false;
		}
		send(lpstruct->m_endpoint, apack->m_buff, apack->m_len);
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
		// 快速flush一次 以更快让客户端收到数据
		lpstruct->flush();
		return ret;
	}

	// ## kcp发送回调会调用的方法
	int asio_kcp::sendbuff(i32_session asession, const char* buf, int len)
	{
		ptr_se lpstruct = m_session.find(asession);
		m_socket.async_send_to(asio::buffer(buf, len), lpstruct->m_endpoint, 
			[](const std::error_code& ec, std::size_t)
			{
				if (ec)
				{
					log_error()->print("asio_kcp::sendbuff error [{}]", ec.message());
				}
			}
		);
		return 0;
	}

	int asio_kcp::sendbuff(const asio_udp_endpoint& aendpoint, const char* buf, int len)
	{
		m_socket.async_send_to(asio::buffer(buf, len), aendpoint, 
			[](const std::error_code& ec, std::size_t)
			{
				if (ec)
				{
					log_error()->print("asio_kcp::sendbuff error [{}]", ec.message());
				}
			}
		);
		return 0;
	}

	void asio_kcp::connect(int32_t aconv
		, std::string& akcpsess
		, i64_actorid aserver
		, i64_actorid aclient
		, const std::string& aip
		, i16_port aport
		, const std::function<void(i32_session)>& afun
	)
	{
		ngl::asio_udp_endpoint lendpoint(asio::ip::address::from_string(aip), aport);
		connect(aconv, akcpsess, aserver, aclient, lendpoint, afun);
	}

	void asio_kcp::connect(int32_t aconv
		, std::string& akcpsess
		, i64_actorid aserver
		, i64_actorid aclient
		, const asio_udp_endpoint& aendpoint
		, const std::function<void(i32_session)>& afun
	)
	{
		// #### 发起连接
		ptr_se lpstruct = m_session.add(aconv, aendpoint, aserver, aclient);
		ncjson ltempjson;
		njson::push(ltempjson.json(), { "actoridserver","actoridclient","session" }, aserver, aclient, akcpsess);
		ltempjson.set_nonformatstr(true);
		udp_cmd::sendcmd(this, lpstruct->m_session, udp_cmd::ecmd_connect, ltempjson.str());
		m_connectfun = afun;
	}

	i64_actorid asio_kcp::find_server(i32_session asession)
	{
		ptr_se lpstruct = m_session.find(asession);
		if (lpstruct == nullptr)
		{
			return -1;
		}
		return lpstruct->m_server;
	}

	i64_actorid asio_kcp::find_client(i32_session asession)
	{
		ptr_se lpstruct = m_session.find(asession);
		if (lpstruct == nullptr)
		{
			return nguid::make();
		}
		return lpstruct->m_client;
	}

	bool asio_kcp::find_actorid(i32_session asession, i64_actorid& aserver, i64_actorid& aclient)
	{
		ptr_se lpstruct = m_session.find(asession);
		if (lpstruct == nullptr)
		{
			return false;
		}
		aactoridserver = lpstruct->m_server;
		aactoridclient = lpstruct->m_client;
		return true;
	}

	i32_session asio_kcp::find_session(i64_actorid aactorid)
	{
		ptr_se lpstruct = m_session.findbyactorid(aactorid);
		if (lpstruct == nullptr)
		{
			return -1;
		}
		return lpstruct->m_session;
	}

	void asio_kcp::close(i32_session asession)
	{
		m_session.erasebysession(asession);
	}

	void asio_kcp::reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient)
	{
		ngl::asio_udp_endpoint lendpoint(asio::ip::address::from_string(aip), aport);
		m_session.reset_add(aconv, lendpoint, aserver, aclient);
	}

	void asio_kcp::close_net(i32_session asession)
	{
		close(asession);
	}
}// namespace ngl