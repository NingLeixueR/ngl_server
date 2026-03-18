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
// File overview: Implements logic for tcp.


#include "actor/protocol/nprotocol.h"
#include "tools/serialize/pack.h"
#include "net/tcp/asio_tcp.h"
#include "net/asio_timer.h"
#include "tools/log/nlog.h"

#include <vector>

namespace ngl
{
	namespace tcp
	{
		constexpr int32_t tcp_connect_interval_ms =
			asio_tcp::etcp_connect_interval * static_cast<int32_t>(localtime::MILLISECOND);

		bool should_ignore_socket_close_error(const basio_errorcode& ec)
		{
			return !ec || ec == basio::error::not_connected || ec == basio::error::operation_aborted;
		}

		bool should_ignore_acceptor_close_error(const basio_errorcode& ec)
		{
			return !ec || ec == basio::error::operation_aborted || ec == basio::error::bad_descriptor;
		}

		std::shared_ptr<basio_tcpacceptor> create_acceptor(
			basio_ioservice& aioservice,
			const basio::ip::tcp& aprotocol,
			i16_port aport
		)
		{
			// Build the acceptor step by step so IPv6 socket options are applied
			// before bind() on platforms where dual-stack is enabled by default.
			auto acceptor = std::make_shared<basio_tcpacceptor>(aioservice);
			acceptor->open(aprotocol);
			if (aprotocol == basio::ip::tcp::v6())
			{
				acceptor->set_option(basio::ip::v6_only(true));
			}
			acceptor->set_option(basio::socket_base::reuse_address(true));
			acceptor->bind(basio_iptcpendpoint(aprotocol, aport));
			acceptor->listen(basio::socket_base::max_listen_connections);
			return acceptor;
		}
	}

	asio_tcp::asio_tcp(
		i16_port aport
		, i32_threadsize athread
		, const tcp_callback& acallfun
		, const tcp_closecallback& aclosefun
		, const tcp_sendfinishcallback& asendfinishfun
	) :
		m_fun(acallfun),
		m_closefun(aclosefun),
		m_sendfinishfun(asendfinishfun),
		m_port(aport),
		m_service_ios(athread, etcp_buffmaxsize),
		m_sessionid(0),
		m_acceptor_v4(nullptr),
		m_acceptor_v6(nullptr)
	{
		basio_ioservice& lioservice = *m_service_ios.get_ioservice(m_service_ios.m_recvthreadsize);
		// One acceptor per address family keeps IPv4 and IPv6 accepts independent.
		m_acceptor_v4 = ngl::tcp::create_acceptor(lioservice, basio::ip::tcp::v4(), m_port);
		if (m_port == 0)
		{
			m_port = m_acceptor_v4->local_endpoint().port();
			log_error()->print("asio_tcp prot preinstall/reality:0/{}", m_port);
		}
		m_acceptor_v6 = ngl::tcp::create_acceptor(lioservice, basio::ip::tcp::v6(), m_port);
		accept(true);
		accept(false);
	}

	asio_tcp::asio_tcp(
		i32_threadsize athread
		, const tcp_callback& acallfun
		, const tcp_closecallback& aclosefun
		, const tcp_sendfinishcallback& asendfinishfun
	) :
		m_fun(acallfun),
		m_closefun(aclosefun),
		m_sendfinishfun(asendfinishfun),
		m_port(0),
		m_service_ios(athread + 1, etcp_buffmaxsize),
		m_sessionid(0),
		m_acceptor_v4(nullptr),
		m_acceptor_v6(nullptr)
	{
	}

	asio_tcp::~asio_tcp()
	{
		basio_errorcode ec;
		if (m_acceptor_v4 != nullptr)
		{
			m_acceptor_v4->close(ec);
			if (!ngl::tcp::should_ignore_acceptor_close_error(ec))
			{
				log_error()->print("asio_tcp::~asio_tcp close v4 acceptor [{}]", ec.message());
			}
			m_acceptor_v4.reset();
		}
		if (m_acceptor_v6 != nullptr)
		{
			m_acceptor_v6->close(ec);
			if (!ngl::tcp::should_ignore_acceptor_close_error(ec))
			{
				log_error()->print("asio_tcp::~asio_tcp close v6 acceptor [{}]", ec.message());
			}
			m_acceptor_v6.reset();
		}

		std::vector<std::shared_ptr<service_tcp>> lservices;
		{
			lock_write(m_maplock);
			lservices.reserve(m_data.size());
			for (auto& [_, service] : m_data)
			{
				if (service != nullptr)
				{
					lservices.push_back(service);
				}
			}
			m_data.clear();
			m_close.clear();
		}
		{
			lock_write(m_ipportlock);
			m_ipport.clear();
		}

		for (auto& service : lservices)
		{
			close_socket(service->m_socket);
		}

		m_service_ios.shutdown();
	}

	service_tcp* asio_tcp::connect(const str_ip& aip, i16_port aport, const tcp_connectcallback& afun, int acount)
	{
		std::shared_ptr<service_tcp> lservice = nullptr;
		{
			lock_write(m_maplock);
			lservice = std::make_shared<service_tcp>(m_service_ios, ++m_sessionid);
			auto [_, success] = m_data.insert(std::make_pair(lservice->m_sessionid, lservice));
			if (!success)
			{
				tools::send_mail("session id repeat")();
				return nullptr;
			}
		}
		lservice->m_socket.async_connect(basio_iptcpendpoint(basio_ipaddress::from_string(aip), aport), 
			[this, lservice, aip, aport, afun, acount](const basio_errorcode& ec)
			{
				if (ec)
				{
					close_net(lservice->m_sessionid);
					if (acount > 0)
					{
						log_error()->print("connect [{}:{}] fail [{}] add timer list! ", aip, aport, ec.message());
						// Retry through the shared timer wheel instead of blocking a socket thread.
						wheel_parm lparm
						{
							.m_ms = ngl::tcp::tcp_connect_interval_ms,
							.m_intervalms = [](int64_t) { return ngl::tcp::tcp_connect_interval_ms; },
							.m_count = 1,
							.m_fun = [this, aip, aport, afun, acount](const wheel_node*)
							{
								connect(aip, aport, afun, acount - 1);
							}
						};
						asio_timer::wheel().addtimer(lparm);
					}
					else if (afun != nullptr)
					{
						afun(-1);
					}
					return;
				}

				basio_errorcode endpoint_ec;
				const auto lremote_endpoint = lservice->m_socket.remote_endpoint(endpoint_ec);
				if (endpoint_ec)
				{
					close_net(lservice->m_sessionid);
					log_error()->print("asio_tcp::connect remote_endpoint [{}]", endpoint_ec.message());
					if (afun != nullptr)
					{
						afun(-1);
					}
					return;
				}
				std::string lip = lremote_endpoint.address().to_string();
				i16_port lport = lremote_endpoint.port();
				bool llanip = tools::is_lanip(lip);
				{
					lock_write(m_ipportlock);
					m_ipport[lservice->m_sessionid] = std::make_pair(lip, lport);
					lservice->m_is_lanip = llanip;
				}
				// Start the read loop only after peer metadata has been cached.
				start(lservice);
				if (afun != nullptr)
				{
					afun(lservice->m_sessionid);
				}
			}
		);
		return lservice.get();
	}

	std::shared_ptr<service_tcp> asio_tcp::get_tcp(i32_sessionid asessionid)
	{
		lock_read(m_maplock);
		std::shared_ptr<service_tcp>* lp = tools::findmap(m_data, asessionid);
		return lp == nullptr ? nullptr : *lp;
	}

	template <typename T>
	bool asio_tcp::spack(i32_sessionid asessionid, std::shared_ptr<T>& apack)
	{
		const std::shared_ptr<service_tcp> tcp = get_tcp(asessionid);
		if (tcp == nullptr)
		{
			return false;
		}
		std::shared_ptr<std::list<node_pack>> llist = nullptr;
		{
			lock_write(tcp->m_mutex);
			tcp->m_list.push_back({ asessionid, apack });
			if (tcp->m_issend == false)
			{
				// Only one async_write chain is allowed at a time per TCP session.
				llist = std::make_shared<std::list<node_pack>>();
				tcp->m_list.swap(*llist);
				tcp->m_issend = true;
			}
		}
		if (llist != nullptr && llist->empty() == false)
		{
			do_send(tcp, llist);
		}
		return true;
	}

	bool asio_tcp::send(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
	{
		return spack(asessionid, apack);
	}

	bool asio_tcp::send(i32_sessionid asessionid, std::shared_ptr<void>& apack)
	{
		return spack(asessionid, apack);
	}

	template <typename TPACK>
	void asio_tcp::async_send(
		const std::shared_ptr<service_tcp>& atcp
		, const std::shared_ptr<std::list<node_pack>>& alist
		, std::shared_ptr<TPACK>& apack
		, char* abuff
		, int32_t abufflen
	)
	{
		atcp->m_socket.async_send(basio::buffer(abuff, abufflen), [this, alist, atcp, apack](const basio_errorcode& ec, std::size_t /*length*/)
			{
				alist->pop_front();
				handle_write(atcp, ec, apack);
				if (ec)
				{
					log_error()->print("asio_tcp::do_send fail [{}]", ec.message().c_str());
					return;
				}
				do_send(atcp, alist);
			}
		);
	}

	void asio_tcp::do_send(const std::shared_ptr<service_tcp>& atcp, const std::shared_ptr<std::list<node_pack>>& alist)
	{
		if (atcp == nullptr)
		{
			return;
		}

		if (alist->empty())
		{
			{
				lock_write(atcp->m_mutex);
				if (atcp->m_list.empty() == false)
				{
					// New writes arrived while the previous chain was in flight.
					atcp->m_list.swap(*alist);
				}
				else
				{
					atcp->m_issend = false;
					return;
				}
			}
		}

		node_pack& item = *alist->begin();
		if (item.is_pack())
		{
			std::shared_ptr<pack>& lpack = item.get_pack();
			int32_t lsize = 0;
			int32_t lpos = 0;
			if (lpack->m_pos != lpack->m_len)
			{
				lsize = lpack->m_len - lpack->m_pos;
				lpos = lpack->m_pos;
			}
			else
			{
				lsize = lpack->m_pos;
				lpos = 0;
			}
			if (lsize < 0)
			{
				close(atcp.get());
				return;
			}
			// Reuse the pack buffer directly; pack::m_pos/m_len tracks partial sends.
			async_send(atcp, alist, lpack, &lpack->m_buff[lpos], lsize);
		}
		else
		{
			std::shared_ptr<void>& lpack = item.get_voidpack();
			pack* lpackptr = static_cast<pack*>(lpack.get());
			async_send(atcp, alist, lpack, lpackptr->m_buff, lpackptr->m_pos);
		}
	}

	void asio_tcp::handle_write(const std::shared_ptr<service_tcp>& atcp, const basio_errorcode& error, std::shared_ptr<pack> apack)
	{
		if (error)
		{
			log_error()->print("asio_tcp::handle_write[{}]", error.message().c_str());
			close(atcp.get());
		}
		if (m_sendfinishfun != nullptr)
		{
			m_sendfinishfun(atcp->m_sessionid, error ? true : false, apack.get());
		}
	}

	void asio_tcp::handle_write(const std::shared_ptr<service_tcp>& atcp, const basio_errorcode& error, std::shared_ptr<void> apack)
	{
		if (error)
		{
			log_error()->print("asio_tcp::handle_write[{}]", error.message().c_str());
			close(atcp.get());
		}
		if (m_sendfinishfun != nullptr)
		{
			m_sendfinishfun(atcp->m_sessionid, error ? true : false, static_cast<pack*>(apack.get()));
		}
	}

	void asio_tcp::close(i32_sessionid sessionid)
	{
		if (sessionid <= 0)
		{
			return;
		}

		// Notify session connection
		std::shared_ptr<service_tcp> lpservice = nullptr;
		std::function<void()> lclosefun = nullptr;
		{
			lock_write(m_maplock);
			tools::erasemap(m_data, sessionid, lpservice);
			tools::erasemap(m_close, sessionid, lclosefun);
		}
		{
			lock_write(m_ipportlock);
			m_ipport.erase(sessionid);
		}

		if (lpservice != nullptr)
		{
			log_error()->print("asio_tcp close sessionid [{}]", sessionid);
			close_socket(lpservice->m_socket);
			if (m_closefun != nullptr)
			{
				m_closefun(sessionid);
			}
		}

		if (lclosefun != nullptr)
		{
			lclosefun();
		}
	}

	void asio_tcp::close(service_tcp* atcp)
	{
		if (atcp == nullptr)
		{
			return;
		}
		close(atcp->m_sessionid);
	}

	void  asio_tcp::close_socket(basio_iptcpsocket& socket)
	{
		basio_errorcode ec;

		// 1: Cancelallasynchronously
		// Cancel pending ops first so their handlers wake up promptly.
		socket.cancel(ec);
		if (!ngl::tcp::should_ignore_socket_close_error(ec))
		{
			log_error()->print("asio_tcp::close_socket cancel [{}]", ec.message());
		}

		// Then shut down the full-duplex TCP stream.
		if (socket.is_open())
		{
			socket.shutdown(basio_iptcpsocket::shutdown_both, ec);
			// "Not_connected" ( close)
			if (!ngl::tcp::should_ignore_socket_close_error(ec))
			{
				log_error()->print("asio_tcp::close_socket shutdown [{}]", ec.message());
			}
		}

		// Finally close the underlying descriptor.
		if (socket.is_open())
		{
			socket.close(ec);
			if (!ngl::tcp::should_ignore_socket_close_error(ec))
			{
				log_error()->print("asio_tcp::close_socket close [{}]", ec.message());
			}
		}
	}

	void asio_tcp::close_net(i32_sessionid sessionid)
	{
		std::shared_ptr<service_tcp> lpservice = nullptr;
		{
			lock_write(m_maplock);
			tools::erasemap(m_data, sessionid, lpservice);
			m_close.erase(sessionid);
		}
		{
			lock_write(m_ipportlock);
			m_ipport.erase(sessionid);
		}
		if (lpservice != nullptr)
		{
			close_socket(lpservice->m_socket);
		}
	}

	bool asio_tcp::get_ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair)
	{
		lock_read(m_ipportlock);
		auto lpair = tools::findmap(m_ipport, assionid);
		if (lpair == nullptr)
		{
			return false;
		}
		apair = *lpair;
		return true;
	}

	void  asio_tcp::accept_handle(bool aisv4, const std::shared_ptr<service_tcp>& aservice, const basio_errorcode& error)
	{
		if (error)
		{
			close_net(aservice->m_sessionid);
			if (error != basio::error::operation_aborted)
			{
				log_error()->print("asio_tcp::accept[{}]", error.message().c_str());
			}
		}
		else
		{
			basio_errorcode endpoint_ec;
			const auto lremote_endpoint = aservice->m_socket.remote_endpoint(endpoint_ec);
			if (endpoint_ec)
			{
				close_net(aservice->m_sessionid);
				log_error()->print("asio_tcp::accept remote_endpoint [{}]", endpoint_ec.message());
				accept(aisv4);
				return;
			}
			std::string lip = lremote_endpoint.address().to_string();
			i16_port lport = lremote_endpoint.port();
			bool llanip = tools::is_lanip(lip);
			{
				lock_write(m_ipportlock);
				m_ipport[aservice->m_sessionid] = std::make_pair(lip, lport);
				aservice->m_is_lanip = llanip;
			}
			// Hand the accepted socket to the read loop after endpoint metadata is cached.
			start(aservice);
		}
		accept(aisv4);
	}

	void asio_tcp::accept(bool av4)
	{
		std::shared_ptr<service_tcp> lservice = nullptr;
		{
			lock_write(m_maplock);
			lservice = std::make_shared<service_tcp>(m_service_ios, ++m_sessionid);
			auto [_, success] = m_data.insert(std::make_pair(lservice->m_sessionid, lservice));
			if (!success)
			{
				tools::send_mail("session id repeat")();
				return;
			}
		}
		if (av4)
		{
			m_acceptor_v4->async_accept(lservice->m_socket, [this, lservice](const basio_errorcode& error)
				{
					accept_handle(true, lservice, error);
				}
			);
		}
		else
		{
			m_acceptor_v6->async_accept(lservice->m_socket, [this, lservice](const basio_errorcode& error)
				{
					accept_handle(false, lservice, error);
				}
			);
		}
	}

	void  asio_tcp::start(const std::shared_ptr<service_tcp>& aservice)
	{
		char* lbuff = aservice->buff();
		aservice->m_socket.async_read_some(basio::buffer(lbuff, m_service_ios.m_buffmaxsize)
			, [this, lbuff, aservice](const basio_errorcode& error, size_t bytes_transferred)
			{
				if (!error)
				{
					// The callback owns framing; false means the higher layer wants the session closed.
					if (m_fun == nullptr || !m_fun(aservice.get(), lbuff, static_cast<uint32_t>(bytes_transferred)))
					{
						close(aservice.get());
					}
					else
					{
						start(aservice);
					}
				}
				else
				{
					// Closeconnection
					close(aservice.get());
					if (error != basio::error::operation_aborted)
					{
						log_error()->print("asio_tcp::handle_read[{}]", error.message().c_str());
					}
				}
			}
		);
	}

	void asio_tcp::set_close(i32_sessionid asession, const std::function<void()>& afun)
	{
		lock_write(m_maplock);
		m_close[asession] = afun;
	}
}// namespace ngl
