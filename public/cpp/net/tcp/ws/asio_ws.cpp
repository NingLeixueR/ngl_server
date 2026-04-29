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
// File overview: Implements websocket transport logic built on TCP.


#include "actor/protocol/nprotocol.h"
#include "tools/tools/tools_time.h"
#include "net/tcp/ws/asio_ws.h"
#include "net/net_session.h"
#include "net/asio_timer.h"

#include <openssl/ssl.h>
#include <type_traits>
#include <utility>
#include <vector>
#include <array>

namespace ngl
{
	namespace ws
	{
		constexpr int32_t ws_connect_interval_ms =
			asio_ws::ews_connect_interval * static_cast<int32_t>(tools::time::MILLISECOND);

		template <typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		template <typename T>
		constexpr bool is_tls_stream_v = std::is_same_v<remove_cvref_t<T>, basio_websocket_tls>;


		bool should_ignore_socket_close_error(const basio_errorcode& ec)
		{
			return !ec
				|| ec == basio::error::not_connected
				|| ec == basio::error::operation_aborted
				|| ec == basio::error::bad_descriptor;
		}

		bool should_ignore_ws_close_error(const basio_errorcode& ec)
		{
			return should_ignore_socket_close_error(ec)
				|| ec == basio::error::eof
				|| ec == basio::ssl::error::stream_truncated
				|| ec == bwebsocket::error::closed;
		}

		bool should_ignore_acceptor_close_error(const basio_errorcode& ec)
		{
			return !ec || ec == basio::error::operation_aborted || ec == basio::error::bad_descriptor;
		}

		bool is_optional_ipv6_acceptor_error(const basio_errorcode& ec)
		{
			return ec == basio::error::address_family_not_supported
				|| ec == boost::system::errc::make_error_code(boost::system::errc::protocol_not_supported)
				|| ec == basio::error::operation_not_supported
				|| ec == boost::system::errc::make_error_code(boost::system::errc::address_not_available);
		}

		std::shared_ptr<basio_tcpacceptor> create_acceptor(
			basio_ioservice& aioservice,
			const basio::ip::tcp& aprotocol,
			i16_port aport
		)
		{
			// Build the acceptor step by step so errors can surface before async accept begins.
			auto acceptor = std::make_shared<basio_tcpacceptor>(aioservice);
			acceptor->open(aprotocol);
			if (aprotocol == basio::ip::tcp::v6())
			{
				// Linux keeps IPv6 sockets dual-stack by default; force v6-only so v4/v6
				// wildcard listeners can share the same port across platforms.
				acceptor->set_option(basio::ip::v6_only(true));
			}
			acceptor->set_option(basio::socket_base::reuse_address(true));
			acceptor->bind(basio_iptcpendpoint(aprotocol, aport));
			acceptor->listen(basio::socket_base::max_listen_connections);
			return acceptor;
		}

		void force_close_socket(basio_iptcpsocket& asocket)
		{
			basio_errorcode ec;
			asocket.cancel(ec);
			if (!should_ignore_socket_close_error(ec))
			{
				log_error()->print("asio_ws::close_socket cancel [{}]", ec.message());
			}

			if (asocket.is_open())
			{
				asocket.shutdown(basio_iptcpsocket::shutdown_both, ec);
				if (!should_ignore_socket_close_error(ec))
				{
					log_error()->print("asio_ws::close_socket shutdown [{}]", ec.message());
				}
			}

			if (asocket.is_open())
			{
				asocket.close(ec);
				if (!should_ignore_socket_close_error(ec))
				{
					log_error()->print("asio_ws::close_socket close [{}]", ec.message());
				}
			}
		}

		void close_stream(service_ws& aservice)
		{
			aservice.visit_stream([](auto& astream)
				{
					// Try the websocket close handshake first before force-closing the TCP socket.
					basio_errorcode ec;
					astream.close(bwebsocket::close_code::normal, ec);
					if (!should_ignore_ws_close_error(ec))
					{
						log_error()->print("asio_ws::close_stream [{}]", ec.message());
					}
				}
			);
		}
	}

	asio_ws::asio_ws(
		i16_port aport,
		i32_threadsize athread,
		bool ause_tls,
		const ws_callback& acallfun,
		const ws_closecallback& aclosefun,
		const ws_sendfinishcallback& asendfinishfun,
		const ws_tls_options& atls_options
	) :
		m_port(aport),
		m_use_tls(ause_tls),
		m_fun(acallfun),
		m_closefun(aclosefun),
		m_sendfinishfun(asendfinishfun),
		m_sessionid(net_session::begin(ENET_WS) - 1),
		m_service_ios(athread, ews_default_recvbuff),
		m_tls_options(atls_options)
	{
		if (m_use_tls)
		{
			// TLS setup is shared across all accepted/client sessions for this asio_ws instance.
			prepare_tls_context(true);
		}

		basio_ioservice& lioservice = *m_service_ios.get_ioservice(m_service_ios.m_recvthreadsize);
		m_acceptor_v4 = ngl::ws::create_acceptor(lioservice, basio::ip::tcp::v4(), m_port);
		if (m_port == 0)
		{
			m_port = m_acceptor_v4->local_endpoint().port();
			log_info()->print("asio_ws port auto-assigned {}", m_port);
		}
		try
		{
			m_acceptor_v6 = ngl::ws::create_acceptor(lioservice, basio::ip::tcp::v6(), m_port);
		}
		catch (const boost::system::system_error& ex)
		{
			if (!ngl::ws::is_optional_ipv6_acceptor_error(ex.code()))
			{
				throw;
			}
			log_warn()->print("asio_ws ipv6 listener unavailable, continue with ipv4 only [{}]", ex.code().message());
			m_acceptor_v6.reset();
		}
		accept(true);
		accept(false);
	}

	asio_ws::asio_ws(
		i32_threadsize athread,
		bool ause_tls,
		const ws_callback& acallfun,
		const ws_closecallback& aclosefun,
		const ws_sendfinishcallback& asendfinishfun,
		const ws_tls_options& atls_options
	) :
		m_use_tls(ause_tls),
		m_fun(acallfun),
		m_closefun(aclosefun),
		m_sendfinishfun(asendfinishfun),
		m_sessionid(net_session::begin(ENET_WS) - 1),
		m_service_ios(athread + 1, ews_default_recvbuff),
		m_tls_options(atls_options)
	{
		if (m_use_tls)
		{
			// TLS setup is shared across all accepted/client sessions for this asio_ws instance.
			prepare_tls_context(false);
		}
	}

	asio_ws::~asio_ws() noexcept
	{
		try
		{

			basio_errorcode ec;
			if (m_acceptor_v4 != nullptr)
			{
				m_acceptor_v4->close(ec);
				if (!ngl::ws::should_ignore_acceptor_close_error(ec))
				{
					log_error()->print("asio_ws::~asio_ws close v4 acceptor [{}]", ec.message());
				}
				m_acceptor_v4.reset();
			}
			if (m_acceptor_v6 != nullptr)
			{
				m_acceptor_v6->close(ec);
				if (!ngl::ws::should_ignore_acceptor_close_error(ec))
				{
					log_error()->print("asio_ws::~asio_ws close v6 acceptor [{}]", ec.message());
				}
				m_acceptor_v6.reset();
			}

			std::vector<std::shared_ptr<service_ws>> lservices;
			{
				lock_write(m_maplock);
				lservices.reserve(m_data.size());
				for (auto& [_, service] : m_data)
				{
					if (service != nullptr)
					{
						lservices.emplace_back(service);
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
				ngl::ws::force_close_socket(service->socket());
			}

			lservices.clear();
			m_service_ios.shutdown();
		}
		catch (...)
		{
			// WebSocket teardown is best-effort during destruction.
		}
	}

	std::shared_ptr<service_ws> asio_ws::create_service()
	{
		lock_write(m_maplock);
		std::shared_ptr<service_ws> lservice = nullptr;
		if (!net_session::next(m_sessionid, ENET_WS))
		{
			log_error()->print("asio_ws::create_service session id exhausted");
			return nullptr;
		}
		if (m_use_tls)
		{
			if (m_tls_context == nullptr)
			{
				log_error()->print("asio_ws::create_service missing tls context");
				return nullptr;
			}
			lservice = std::make_shared<service_ws>(m_service_ios, m_sessionid, *m_tls_context);
		}
		else
		{
			lservice = std::make_shared<service_ws>(m_service_ios, m_sessionid);
		}

		auto [_, success] = m_data.emplace(lservice->m_sessionid, lservice);
		if (!success)
		{
			log_error()->print("asio_ws::create_service session id repeat [{}]", lservice->m_sessionid);
			return nullptr;
		}
		return lservice;
	}

	std::shared_ptr<service_ws> asio_ws::get_ws(i32_sessionid asessionid)
	{
		lock_read(m_maplock);
		auto it = m_data.find(asessionid);
		return it == m_data.end() ? nullptr : it->second;
	}

	void asio_ws::prepare_tls_context(bool aserver)
	{
		try
		{
			m_tls_context = std::make_unique<basio_sslcontext>(
				aserver ? basio::ssl::context::tls_server : basio::ssl::context::tls_client
			);
			m_tls_context->set_options(
				basio::ssl::context::default_workarounds
				| basio::ssl::context::no_sslv2
				| basio::ssl::context::no_sslv3
				| basio::ssl::context::single_dh_use
			);

			if (aserver)
			{
				if (m_tls_options.m_certificate_chain.empty() || m_tls_options.m_private_key.empty())
				{
					log_error()->print("asio_ws::prepare_tls_context missing server certificate or private key");
					return;
				}

				m_tls_context->use_certificate_chain(
					basio::buffer(m_tls_options.m_certificate_chain.data(), m_tls_options.m_certificate_chain.size())
				);
				m_tls_context->use_private_key(
					basio::buffer(m_tls_options.m_private_key.data(), m_tls_options.m_private_key.size()),
					basio::ssl::context::pem
				);
			}
			else if (m_tls_options.m_verify_peer)
			{
				// Client mode can rely on system trust plus optional caller-supplied CA data.
				m_tls_context->set_default_verify_paths();
				m_tls_context->set_verify_mode(basio::ssl::verify_peer);
				if (!m_tls_options.m_ca_certificates.empty())
				{
					m_tls_context->add_certificate_authority(
						basio::buffer(m_tls_options.m_ca_certificates.data(), m_tls_options.m_ca_certificates.size())
					);
				}
			}
			else
			{
				m_tls_context->set_verify_mode(basio::ssl::verify_none);
			}
		}
		catch (const std::exception& ex)
		{
			log_error()->print("asio_ws::prepare_tls_context [{}]", ex.what());
		}
	}

	void asio_ws::configure_websocket(const std::shared_ptr<service_ws>& aservice, bool aserver)
	{
		if (aservice == nullptr)
		{
			return;
		}

		aservice->visit_stream([aserver](auto& astream)
			{
				// Beast timeout presets cover handshake, idle, and close timeouts.
				astream.set_option(bwebsocket::stream_base::timeout::suggested(
					aserver ? beast::role_type::server : beast::role_type::client
				));
				if (aserver)
				{
					astream.set_option(bwebsocket::stream_base::decorator(
						[](bwebsocket::response_type& res)
						{
							res.set(boost::beast::http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " ngl-server");
						}
					));
				}
				else
				{
					astream.set_option(bwebsocket::stream_base::decorator(
						[](bwebsocket::request_type& req)
						{
							req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " ngl-client");
						}
					));
				}
				astream.binary(true);
			}
		);
	}

	bool asio_ws::cache_remote_endpoint(const std::shared_ptr<service_ws>& aservice)
	{
		if (aservice == nullptr)
		{
			return false;
		}

		basio_errorcode ec;
		const auto endpoint = aservice->socket().remote_endpoint(ec);
		if (ec)
		{
			log_error()->print("asio_ws::cache_remote_endpoint [{}]", ec.message());
			return false;
		}

		std::string lip = endpoint.address().to_string();
		const bool llanip = tools::is_lanip(lip);
		{
			lock_write(m_ipportlock);
			m_ipport.insert_or_assign(aservice->m_sessionid, std::make_pair(std::move(lip), endpoint.port()));
			aservice->m_is_lanip = llanip;
		}
		return true;
	}

	void asio_ws::handle_connect_failure(
		const std::shared_ptr<service_ws>& aservice,
		const basio_errorcode& aerror,
		const str_host& ahost,
		i16_port aport,
		const std::string& atarget,
		const ws_connectcallback& afun,
		int acount,
		const char* astage
	)
	{

		if (aservice != nullptr)
		{
			close_net(aservice->m_sessionid);
		}

		log_error()->print("asio_ws::connect [{}:{}] stage[{}] fail [{}]", ahost, aport, astage, aerror.message());

		if (acount > 0)
		{
			// Retry failures through the shared timer wheel rather than blocking socket threads.
			tools::wheel_parm lparm
			{
				.m_ms = ngl::ws::ws_connect_interval_ms,
				.m_intervalms = [](int64_t) { return  ngl::ws::ws_connect_interval_ms; },
				.m_count = 1,
				.m_fun = [this, ahost, aport, atarget, afun, acount](const tools::wheel_node*)
				{
					connect(ahost, aport, atarget, afun, acount - 1);
				}
			};
			asio_timer::wheel().addtimer(lparm);
		}
		else if (afun != nullptr)
		{
			afun(-1);
		}
	}

	void asio_ws::begin_server_handshake(const std::shared_ptr<service_ws>& aservice)
	{
		if (aservice == nullptr)
		{
			return;
		}

		configure_websocket(aservice, true);
		if (aservice->using_tls())
		{
			aservice->visit_stream([this, aservice](auto& astream)
				{
					using stream_type = ngl::ws::remove_cvref_t<decltype(astream)>;
					if constexpr (ngl::ws::is_tls_stream_v<stream_type>)
					{
						// Server-side WSS performs TLS handshake first, then websocket upgrade.
						astream.next_layer().async_handshake(
							basio::ssl::stream_base::server,
							[this, aservice](const basio_errorcode& ec)
							{
								if (ec)
								{
									handle_connect_failure(aservice, ec, "server", m_port, "/", nullptr, 0, "tls-accept");
									return;
								}

								aservice->visit_stream([this, aservice](auto& aws_stream)
									{
										aws_stream.async_accept(
											[this, aservice](const basio_errorcode& ws_ec)
											{
												if (ws_ec)
												{
													handle_connect_failure(aservice, ws_ec, "server", m_port, "/", nullptr, 0, "ws-accept");
													return;
												}
												if (!cache_remote_endpoint(aservice))
												{
													close_net(aservice->m_sessionid);
													return;
												}
												start(aservice);
											}
										);
									}
								);
							}
						);
					}
				}
			);
			return;
		}

		aservice->visit_stream([this, aservice](auto& astream)
			{
				astream.async_accept(
					[this, aservice](const basio_errorcode& ec)
					{
						if (ec)
						{
							handle_connect_failure(aservice, ec, "server", m_port, "/", nullptr, 0, "ws-accept");
							return;
						}
						if (!cache_remote_endpoint(aservice))
						{
							close_net(aservice->m_sessionid);
							return;
						}
						start(aservice);
					}
				);
			}
		);
	}

	void asio_ws::begin_client_handshake(
		const std::shared_ptr<service_ws>& aservice,
		const str_host& ahost,
		const std::string& atarget,
		const ws_connectcallback& afun,
		i16_port aport,
		int acount
	)
	{
		if (aservice == nullptr)
		{
			if (afun != nullptr)
			{
				afun(-1);
			}
			return;
		}

		configure_websocket(aservice, false);
		if (aservice->using_tls())
		{
			aservice->visit_stream([this, aservice, ahost, atarget, afun, aport, acount](auto& astream)
				{
					using stream_type = ngl::ws::remove_cvref_t<decltype(astream)>;
					if constexpr (ngl::ws::is_tls_stream_v<stream_type>)
					{
						if (!ahost.empty())
						{
							// SNI is required by many hosted TLS endpoints.
							SSL_set_tlsext_host_name(astream.next_layer().native_handle(), ahost.c_str());
						}

						// Client-side WSS performs TCP connect -> TLS handshake -> websocket upgrade.
						astream.next_layer().async_handshake(
							basio::ssl::stream_base::client,
							[this, aservice, ahost, atarget, afun, aport, acount](const basio_errorcode& ec)
							{
								if (ec)
								{
									handle_connect_failure(aservice, ec, ahost, aport, atarget, afun, acount, "tls-connect");
									return;
								}

								aservice->visit_stream([this, aservice, ahost, atarget, afun, aport, acount](auto& aws_stream)
									{
										aws_stream.async_handshake(
											ahost,
											atarget,
											[this, aservice, ahost, aport, atarget, afun, acount](const basio_errorcode& ws_ec)
											{
												if (ws_ec)
												{
													handle_connect_failure(aservice, ws_ec, ahost, aport, atarget, afun, acount, "ws-connect");
													return;
												}
												if (!cache_remote_endpoint(aservice))
												{
													close_net(aservice->m_sessionid);
													if (afun != nullptr)
													{
														afun(-1);
													}
													return;
												}
												start(aservice);
												if (afun != nullptr)
												{
													afun(aservice->m_sessionid);
												}
											}
										);
									}
								);
							}
						);
					}
				}
			);
			return;
		}

		aservice->visit_stream([this, aservice, ahost, atarget, afun, aport, acount](auto& astream)
			{
				astream.async_handshake(
					ahost,
					atarget,
					[this, aservice, ahost, aport, atarget, afun, acount](const basio_errorcode& ec)
					{
						if (ec)
						{
							handle_connect_failure(aservice, ec, ahost, aport, atarget, afun, acount, "ws-connect");
							return;
						}
						if (!cache_remote_endpoint(aservice))
						{
							close_net(aservice->m_sessionid);
							if (afun != nullptr)
							{
								afun(-1);
							}
							return;
						}
						start(aservice);
						if (afun != nullptr)
						{
							afun(aservice->m_sessionid);
						}
					}
				);
			}
		);
	}


	template <typename T>
	bool asio_ws::spack(i32_sessionid asessionid, std::shared_ptr<T>& apack)
	{
		const std::shared_ptr<service_ws> ws = get_ws(asessionid);
		if (ws == nullptr)
		{
			return false;
		}
		ws->m_npacklist.push(apack);
		basio::post(ws->m_ioservice, [this, ws]()
		{
			do_send(ws);
		});
		return true;
	}

	void asio_ws::async_send(const std::shared_ptr<service_ws>& aservice, const std::shared_ptr<node_pack>& anodepack)
	{
		aservice->visit_stream([this, aservice, anodepack](auto& astream)
			{
				if (anodepack->head() != nullptr)
				{
					astream.binary(true);
					std::array<basio::const_buffer, 2> lbufs{
						basio::buffer(anodepack->head_data(), anodepack->head_byte()),
						basio::buffer(anodepack->buff(), anodepack->pos())
					};
					send(aservice, astream, anodepack, lbufs);
				}
				else
				{
					astream.binary(false);
					send(aservice, astream, anodepack, basio::buffer(anodepack->buff(), anodepack->pos()));
				}
			}
		);
	}

	void asio_ws::do_send(const std::shared_ptr<service_ws>& aservice, bool async/* = false*/)
	{
		if (aservice == nullptr)
		{
			return;
		}

		aservice->m_npacklist.send(async, aservice, this, &asio_ws::async_send);
	}

	void asio_ws::handle_write(const std::shared_ptr<service_ws>& aservice, const basio_errorcode& error, const std::shared_ptr<pack>& apack)
	{
		if (error)
		{
			if (!ngl::ws::should_ignore_ws_close_error(error))
			{
				log_error()->print("asio_ws::handle_write [{}]", error.message());
			}
			close(aservice.get());
		}
		if (m_sendfinishfun != nullptr)
		{
			m_sendfinishfun(aservice->m_sessionid, error ? true : false, apack.get());
		}
	}

	void asio_ws::accept_handle(bool av4, const std::shared_ptr<service_ws>& aservice, const basio_errorcode& error)
	{
		if (error)
		{
			close_net(aservice->m_sessionid);
			if (error != basio::error::operation_aborted)
			{
				log_error()->print("asio_ws::accept [{}]", error.message());
			}
		}
		else
		{
			begin_server_handshake(aservice);
		}

		accept(av4);
	}

	void asio_ws::accept(bool av4)
	{

		if (av4)
		{
			if (m_acceptor_v4 == nullptr || !m_acceptor_v4->is_open())
			{
				return;
			}
		}
		else
		{
			if (m_acceptor_v6 == nullptr || !m_acceptor_v6->is_open())
			{
				return;
			}
		}

		const std::shared_ptr<service_ws> lservice = create_service();
		if (lservice == nullptr)
		{
			return;
		}

		if (av4)
		{
			m_acceptor_v4->async_accept(lservice->socket(),
				[this, lservice, av4](const basio_errorcode& error)
				{
					accept_handle(av4, lservice, error);
				}
			);
		}
		else
		{
			m_acceptor_v6->async_accept(lservice->socket(),
				[this, lservice, av4](const basio_errorcode& error)
				{
					accept_handle(av4, lservice, error);
				}
			);
		}		
	}

	void asio_ws::start(const std::shared_ptr<service_ws>& aservice)
	{
		if (aservice == nullptr)
		{
			return;
		}

		aservice->visit_stream([this, aservice](auto& astream)
			{
				astream.async_read(
					aservice->read_buffer(),
					[this, aservice](const basio_errorcode& error, std::size_t bytes_transferred)
					{
						if (!error)
						{
							const beast::flat_buffer::const_buffers_type ldata = aservice->read_buffer().cdata();
							const char* lbuf = static_cast<const char*>(ldata.data());
							const uint32_t llen = static_cast<uint32_t>(bytes_transferred);
							const bool lkeep = m_fun != nullptr && m_fun(aservice.get(), lbuf, llen);
							aservice->consume_read_buffer(bytes_transferred);

							// m_fun owns framing/dispatch; false means the higher layer wants the session closed.
							if (!lkeep)
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
							close(aservice.get());
							if (!ngl::ws::should_ignore_ws_close_error(error))
							{
								log_error()->print("asio_ws::handle_read [{}]", error.message());
							}
						}
					}
				);
			}
		);
	}

	void asio_ws::close_socket(basio_iptcpsocket& asocket)
	{
		ngl::ws::force_close_socket(asocket);
	}

	i16_port asio_ws::port() const
	{
		return m_port;
	}

	service_ws* asio_ws::connect(const str_host& ahost, i16_port aport, const std::string& atarget, const ws_connectcallback& afun, int acount)
	{
		const std::shared_ptr<service_ws> lservice = create_service();
		if (lservice == nullptr)
		{
			if (afun != nullptr)
			{
				afun(-1);
			}
			return nullptr;
		}

		basio_errorcode address_ec;
		const basio_ipaddress laddress = basio_ipaddress::from_string(ahost, address_ec);
		if (address_ec)
		{
			close_net(lservice->m_sessionid);
			log_error()->print("asio_ws::connect invalid address [{}] [{}]", ahost, address_ec.message());
			if (afun != nullptr)
			{
				afun(-1);
			}
			return nullptr;
		}

		lservice->socket().async_connect(
			basio_iptcpendpoint(laddress, aport),
			[this, lservice, ahost, aport, atarget, afun, acount](const basio_errorcode& ec)
			{
				if (ec)
				{
					handle_connect_failure(lservice, ec, ahost, aport, atarget, afun, acount, "tcp-connect");
					return;
				}
				begin_client_handshake(lservice, ahost, atarget, afun, aport, acount);
			}
		);
		return lservice.get();
	}

	service_ws* asio_ws::connect(const str_host& ahost, i16_port aport, const ws_connectcallback& afun, int acount)
	{
		return connect(ahost, aport, "/", afun, acount);
	}

	bool asio_ws::send(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
	{
		return spack(asessionid, apack);
	}

	bool asio_ws::send(i32_sessionid asessionid, std::shared_ptr<void>& apack)
	{
		return spack(asessionid, apack);
	}

	bool asio_ws::sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
	{
		return send(asessionid, apack);
	}

	bool asio_ws::sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack)
	{
		return send(asessionid, apack);
	}

	void asio_ws::close(i32_sessionid sessionid)
	{
		close_session(sessionid, false, true, true);
	}

	void asio_ws::close_session(i32_sessionid sessionid, bool agraceful, bool anotifyclose, bool anotifycallback)
	{
		if (sessionid <= 0)
		{
			return;
		}

		std::shared_ptr<service_ws> lservice = nullptr;
		std::function<void()> lclosefun = nullptr;
		{
			lock_write(m_maplock);
			auto it = m_data.find(sessionid);
			if (it != m_data.end())
			{
				lservice = it->second;
				m_data.erase(it);
			}
			auto close_it = m_close.find(sessionid);
			if (close_it != m_close.end())
			{
				lclosefun = close_it->second;
				m_close.erase(close_it);
			}
		}
		{
			lock_write(m_ipportlock);
			m_ipport.erase(sessionid);
		}

		const ws_closecallback lnotifyclose = m_closefun;

		if (lservice != nullptr)
		{
			if (agraceful)
			{
				ngl::ws::close_stream(*lservice);
			}
			ngl::ws::force_close_socket(lservice->socket());

			if (anotifyclose && lnotifyclose != nullptr)
			{
				lnotifyclose(sessionid);
			}
		}

		if (anotifycallback && lclosefun != nullptr)
		{
			lclosefun();
		}
	}

	void asio_ws::close(service_ws* asession)
	{
		if (asession == nullptr)
		{
			return;
		}
		close_session(asession->m_sessionid, false, true, true);
	}

	void asio_ws::close_net(i32_sessionid sessionid)
	{
		close_session(sessionid, false, false, false);
	}

	bool asio_ws::get_ipport(i32_sessionid asessionid, std::pair<str_ip, i16_port>& apair)
	{
		lock_read(m_ipportlock);
		auto it = m_ipport.find(asessionid);
		if (it == m_ipport.end())
		{
			return false;
		}
		apair = it->second;
		return true;
	}

	bool asio_ws::sessionid2ipport(i32_sessionid asessionid, std::pair<str_ip, i16_port>& apair)
	{
		return get_ipport(asessionid, apair);
	}

	bool asio_ws::exist_session(i32_sessionid asessionid)
	{
		lock_read(m_maplock);
		return m_data.find(asessionid) != m_data.end();
	}

	void asio_ws::set_close(i32_sessionid asession, const std::function<void()>& afun)
	{
		lock_write(m_maplock);
		m_close[asession] = afun;
	}
}// namespace ngl
