/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for kcp.

#include "tools/tab/xml/sysconfig.h"
#include "actor/protocol/protocol.h"
#include "tools/tools/tools_cmd.h"
#include "net/udp/kcp/asio_kcp.h"
#include "net/udp/kcp/ukcp.h"

#include <vector>

#define USE_WHEEL_TIMER

namespace ngl
{
	// Register the inbound KCP handshake request handler.
	void asio_kcp::func_ecmd_connect()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_connect, [](asio_kcp* ap, std::shared_ptr<kcp_endpoint>& apstruct, const std::string& ajson)
			{
				// The first control packet authenticates the logical server/client pair.
				apstruct->m_isconnect = true;
				ncjson ltempjson(ajson.c_str());

				i64_actorid lclient = nguid::make();
				i64_actorid lserver = nguid::make();
				std::string lsession;
				if (!njson::pop(ltempjson, { "actoridclient","actoridserver","session" }, lclient, lserver, lsession))
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

				// Acknowledge the handshake over the same KCP control channel.
				udp_cmd::sendcmd(ap, apstruct->m_session, udp_cmd::ecmd_connect_ret, "{}");
			}
		);
	}

	// Register the handshake acknowledgement handler for outbound connect attempts.
	void asio_kcp::func_ecmd_connect_ret()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_connect_ret, [](asio_kcp* ap, std::shared_ptr<kcp_endpoint>& apstruct, const std::string&)
			{
				apstruct->m_isconnect = true;
				if (ap->m_connectfun != nullptr)
				{
					// Outbound callers are released only after the remote side acknowledged the handshake.
					ap->m_connectfun(apstruct->m_session);
				}
			}
		);
	}

	// Register the peer-initiated close handler.
	void asio_kcp::func_ecmd_close()const
	{
		udp_cmd::register_fun(udp_cmd::ecmd_close, [](asio_kcp* ap, std::shared_ptr<kcp_endpoint>& apstruct, const std::string&)
			{
				udp_cmd::sendcmd(ap, apstruct->m_session, udp_cmd::ecmd_close_ret, "");
				int lession = apstruct->m_session;
				tools::wheel_parm lparm
				{
					.m_ms = e_close_intervalms,
					.m_intervalms = [](int64_t) {return e_close_intervalms; },
					.m_count = 0x7fffffff,
					.m_fun = [ap,lession](const tools::wheel_node*)
					{
						// Defer actual teardown slightly so the close reply has time to flush out.
						ap->close(lession);
					}
				};
				m_kcptimer.addtimer(lparm);
			}
		);
	}

	asio_kcp::asio_kcp(i16_port port)
		: m_session(this)
		, m_context()
		, m_work_guard(basio::make_work_guard(m_context))
		, m_socket(m_context, basio::ip::udp::endpoint(basio::ip::udp::v4(), port))
		, m_port(port)
	{
		func_ecmd_connect();
		func_ecmd_connect_ret();
		func_ecmd_close();

		// Begin receiving immediately; the io_context thread starts right after.
		start();
		m_thread = std::jthread([this](std::stop_token)
			{
				m_context.run();
			}
		);
	}

	asio_kcp::~asio_kcp() noexcept
	{
		try
		{
			{
				std::scoped_lock llock(m_waitmutex);
				m_wait = nullptr;
				m_waitendpoint = basio::ip::udp::endpoint();
			}

			basio_errorcode ec;
			m_socket.cancel(ec);
			m_socket.close(ec);
			m_work_guard.reset();
			m_context.stop();
		}
		catch (...)
		{
			// UDP/KCP teardown should never propagate exceptions during destruction.
		}
	}

	bool asio_kcp::async_send(const std::shared_ptr<kcp_endpoint>& akcp, const std::shared_ptr<pack>& apack)
	{
		if (apack->m_head != nullptr)
		{
			std::array<basio::const_buffer, 2> bufs{
				basio::buffer(apack->m_head->m_data, sizeof(apack->m_head->m_data)),
				basio::buffer(apack->m_buff, apack->m_pos)
			};
			return send(akcp, apack, bufs);
		}
		else
		{
			return send(akcp, apack, basio::buffer(apack->m_buff, apack->m_pos));
		}
	}

	bool asio_kcp::do_send(const std::shared_ptr<kcp_endpoint>& akcp)
	{
		if (akcp == nullptr)
		{
			return false;
		}

		node_pack lnoepack;
		{
			std::lock_guard<std::mutex> llock(akcp->m_mutex);
			if (akcp->m_list.empty())
			{
				akcp->m_issend = false;
				return true;
			}
			if (akcp->m_issend)
			{
				return true;
			}
			akcp->m_issend = true;
			lnoepack = std::move(akcp->m_list.front());
			akcp->m_list.pop_front();
		}
		return async_send(akcp, lnoepack.get_pack());
	}

	bool asio_kcp::send(i32_sessionid asession, std::shared_ptr<pack>& apack)
	{
		return send(asession, apack->m_buff, apack->m_pos);
	}

	bool asio_kcp::send(i32_sessionid asessionid, const char* buf, int32_t len)
	{
		std::shared_ptr<kcp_endpoint> lpstruct = m_session.find(asessionid);
		if (lpstruct == nullptr)
		{
			return false;
		}
		int ret = lpstruct->send(buf, len);
		// Flush immediately so KCP emits UDP packets without waiting for the next update tick.
		lpstruct->flush();
		return ret>=0;
	}

	bool asio_kcp::sempack(const std::shared_ptr<kcp_endpoint>& apstruct, const char* abuff, int abufflen)
	{
		if (!m_rate.add(apstruct->m_session))
		{
			return false;
		}
		// Parse the serialized pack header first so we know the expected packet size.
		std::shared_ptr<pack> lpack = pack::make_pack(&m_pool, 0);
		if (lpack == nullptr)
		{
			return false;
		}
		lpack->m_protocol = ENET_KCP;
		lpack->m_id = apstruct->m_session;
		if (EPH_HEAD_SUCCESS != lpack->m_head->push(abuff, abufflen))
		{
			return false;
		}
		int len = lpack->m_head->getvalue(EPH_BYTES);
		if (len < 0 || len != abufflen)
		{
			return false;
		}
		if (!lpack->malloc(len) || lpack->m_buff == nullptr)
		{
			return false;
		}
		memcpy(lpack->m_buff, abuff, len);
		
		if (nconfig.nodetype() != ROBOT)
		{
			// Server-side KCP traffic rewrites actor headers to the logical server/client pair.
			lpack->m_head->set_actor(apstruct->m_server, apstruct->m_client);
		}
		
		lpack->m_pos = len;
		if (tools::time::gettime() < lpack->m_head->getvalue(EPH_TIME) + sysconfig::net_timeout())
		{
			// Valid KCP payloads re-enter the same protocol dispatch path as TCP payloads.
			protocol::push(lpack);
		}
		else
		{
			log_error()->print("time[{} < {} + {} ]", tools::time::gettime(), lpack->m_head->getvalue(EPH_TIME), sysconfig::net_timeout());
		}

		return true;
	}

	void asio_kcp::start()
	{
		m_socket.async_receive_from(basio::buffer(m_buff, e_buff_byte), m_remoteport,
			[this](const basio_errorcode& ec, std::size_t bytes_received)
			{
				if (!ec && bytes_received > 0)
				{
					std::function<void(char*, int)> lwait = nullptr;
					{
						std::lock_guard<std::mutex> llock(m_waitmutex);
						if (m_wait != nullptr && m_remoteport == m_waitendpoint)
						{
							lwait = m_wait;
							m_waitendpoint = basio::ip::udp::endpoint();
							m_wait = nullptr;
						}
					}

					if (lwait != nullptr)
					{
						// Raw UDP request/response helpers bypass KCP session lookup entirely.
						lwait(m_buff, static_cast<int>(bytes_received));
					}
					else
					{
						std::shared_ptr<kcp_endpoint> lpstruct = m_session.find(m_remoteport);
						if (lpstruct != nullptr)
						{
							int linput = lpstruct->input(m_buff, (long)bytes_received);
							if (linput >= 0)
							{
								while (true)
								{
									// Drain every complete KCP message currently available after input().
									int lrecv = lpstruct->recv(m_buffrecv, e_buffrecv_byte);
									if (lrecv == -3)
									{
										// Ret == -3 m_buffrecv
										close(lpstruct->m_session);
										break;
									}
									if (lrecv < 0)
									{
										break;
									}

									// Control commands are sent through the same KCP stream with an "ecmd*" prefix.
									if (udp_cmd::run_cmd(this, lpstruct, m_buffrecv, lrecv))
									{
										log_error()->print("kcp cmd [{}]", std::string(m_buffrecv, lrecv));
										break;
									}

									if (lpstruct->m_isconnect == false)
									{
										// Drop data packets until the logical handshake finishes.
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
									// Debug helper used to learn the public UDP endpoint from the peer.
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
								// "NFC" means the peer has no known KCP session for this endpoint.
								sendu(m_remoteport, "NFC", sizeof("NFC"));
							}
						}
					}
				}
				else if (ec && ec != basio::error::operation_aborted)
				{
					log_error()->print("asio_kcp::receive error [{}]", ec.message());
				}

				if (m_socket.is_open() && ec != basio::error::operation_aborted)
				{
					start();
				}
			});
	};

	bool asio_kcp::async_send(const basio::ip::udp::endpoint& aendpoint, const std::shared_ptr<pack>& apack)
	{
		m_socket.async_send_to(basio::buffer(apack->m_buff, apack->m_pos), aendpoint,
			[this, apack](const basio_errorcode& ec, std::size_t)
			{
				if (ec)
				{
					log_error()->print("asio_kcp::async_send2[{}]", ec.message().c_str());
				}
				do_send();
			}
		);
		return true;
	}

	bool asio_kcp::do_send()
	{
		tmpdata litem;
		{
			std::lock_guard<std::mutex> llock(m_mutex);
			if (m_list.empty())
			{
				m_issend = false;
				return true;
			}
			if (m_issend)
			{
				return true;
			}
			litem = std::move(m_list.front());
			m_list.pop_front();
			m_issend = true;
		}
		return async_send(litem.m_endpoint, litem.m_pack);
	}

	bool asio_kcp::sendu(const basio::ip::udp::endpoint& aendpoint, const char* buf, int32_t len)
	{
		auto lpack = pack::make_pack(&m_pool, len);
		if (lpack == nullptr || lpack->m_buff == nullptr)
		{
			return false;
		}
		lpack->m_head = nullptr;
		memcpy(lpack->m_buff, buf, len);
		lpack->m_len = len;
		lpack->m_pos = len;
		return sendu(aendpoint, lpack);
	}

	bool asio_kcp::sendu(kcp_endpoint* akcpe, const char* buf, int32_t len)
	{
		return sendu(akcpe->m_endpoint, buf, len);
	}

	bool asio_kcp::sendu(const basio::ip::udp::endpoint& aendpoint, const std::shared_ptr<pack>& apack)
	{
		{
			std::lock_guard<std::mutex> llock(m_mutex);
			m_list.push_back(tmpdata
				{
					.m_endpoint = aendpoint,
					.m_pack = apack
				});
		}
		return do_send();
	}

	bool asio_kcp::sendu(kcp_endpoint* akcpe, const std::shared_ptr<pack>& apack)
	{
		return async_send(akcpe->shared_from_this(), apack);
	}

	bool asio_kcp::sendu_waitrecv(const basio::ip::udp::endpoint& aendpoint, const std::shared_ptr<pack>& apack)
	{
		m_socket.async_send_to(basio::buffer(apack->m_buff, apack->m_pos), aendpoint, [this, aendpoint, apack](const basio_errorcode& ec, std::size_t)
			{
				if (ec)
				{
					log_error()->print("async_send_to err [{}]", ec.message());

					tools::wheel_parm lparm
					{
						.m_ms = e_waitrecv_intervalms,
						.m_intervalms = [](int64_t) {return e_waitrecv_intervalms; } ,
						.m_count = 1,
						.m_fun = [this, aendpoint, apack](const tools::wheel_node*)
						{
							// Retry the raw UDP request until one send actually leaves the socket.
							sendu_waitrecv(aendpoint, apack);
						}
					};
					m_kcptimer.addtimer(lparm);
				}
			}
		);
		return true;
	}

	bool asio_kcp::sendu_waitrecv(const basio::ip::udp::endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun)
	{
		{
			std::lock_guard<std::mutex> llock(m_waitmutex);
			// Only one synchronous raw-UDP waiter is tracked at a time.
			m_wait = afun;
			m_waitendpoint = aendpoint;
		}

		auto lpack = pack::make_pack(&m_pool, len);
		if (lpack == nullptr || lpack->m_buff == nullptr)
		{
			return false;
		}
		lpack->m_head = nullptr;
		memcpy(lpack->m_buff, buf, len);
		lpack->m_len = len;
		lpack->m_pos = len;
		return sendu_waitrecv(aendpoint, lpack);
	}

	bool asio_kcp::send(std::shared_ptr<pack>& apack)
	{
		m_session.foreach([this, &apack](std::shared_ptr<kcp_endpoint>& aptr)
			{
				send(aptr->m_session, apack);
			}
		);
		return true;
	}

	bool asio_kcp::sendpackbyarea(i16_area aarea, std::shared_ptr<pack>& apack)
	{
		m_session.foreachbyarea(aarea, [this, &apack](std::shared_ptr<kcp_endpoint>& aptr)
			{
				send(aptr->m_session, apack);
			}
		);
		return true;
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
		basio::ip::udp::endpoint lendpoint(basio_ipaddress::from_string(aip), aport);
		connect(aconv, akcpsess, aserver, aclient, lendpoint, afun);
	}

	void asio_kcp::connect(int32_t aconv
		, std::string& akcpsess
		, i64_actorid aserver
		, i64_actorid aclient
		, const basio::ip::udp::endpoint& aendpoint
		, const std::function<void(i32_session)>& afun
	)
	{
		// Build the local KCP session first, then send the logical handshake command over it.
		std::shared_ptr<kcp_endpoint> lpstruct = m_session.add(aconv, aendpoint, aserver, aclient);
		ncjson ltempjson;
		njson::push(ltempjson, { "actoridserver","actoridclient","session" }, aserver, aclient, akcpsess);
		m_connectfun = afun;
		udp_cmd::sendcmd(this, lpstruct->m_session, udp_cmd::ecmd_connect, ltempjson.nonformat_str());
	}

	i64_actorid asio_kcp::find_server(i32_session asession)
	{
		std::shared_ptr<kcp_endpoint> lpstruct = m_session.find(asession);
		if (lpstruct == nullptr)
		{
			return -1;
		}
		return lpstruct->m_server;
	}

	i64_actorid asio_kcp::find_client(i32_session asession)
	{
		std::shared_ptr<kcp_endpoint> lpstruct = m_session.find(asession);
		if (lpstruct == nullptr)
		{
			return nguid::make();
		}
		return lpstruct->m_client;
	}

	bool asio_kcp::find_actorid(i32_session asession, i64_actorid& aserver, i64_actorid& aclient)
	{
		std::shared_ptr<kcp_endpoint> lpstruct = m_session.find(asession);
		if (lpstruct == nullptr)
		{
			return false;
		}
		aserver = lpstruct->m_server;
		aclient = lpstruct->m_client;
		return true;
	}

	i32_session asio_kcp::find_session(i64_actorid aactorid)
	{
		std::shared_ptr<kcp_endpoint> lpstruct = m_session.findbyactorid(aactorid);
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
		basio::ip::udp::endpoint lendpoint(basio_ipaddress::from_string(aip), aport);
		m_session.reset_add(aconv, lendpoint, aserver, aclient);
	}

	void asio_kcp::close_net(i32_session asession)
	{
		close(asession);
	}
}// namespace ngl
