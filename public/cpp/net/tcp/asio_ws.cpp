#include "asio_ws.h"
#include "asio_timer.h"
#include "nlog.h"
#include "pack.h"

namespace ngl
{	
	struct asio_ws::impl_asio_ws
	{
		boost::asio::ip::tcp::acceptor*					m_acceptor;
		i16_port										m_port;
		ws_callback										m_fun;
		ws_closecallback								m_closefun;
		ws_sendfinishcallback							m_sendfinishfun;
		std::unordered_map<i32_sessionid, service_ws*>	m_data;
		std::shared_mutex								m_maplock;
		serviceio_info									m_service_io_;
		std::unordered_map<i32_sessionid, std::pair<str_ip, i16_port>> m_ipport;
		std::unordered_map<i32_sessionid, std::function<void()>> m_sessionclose;
		std::shared_mutex								m_ipportlock;

		impl_asio_ws(
			i16_port aport
			, i32_threadsize athread
			, const ws_callback& acallfun
			, const ws_closecallback& aclosefun
			, const ws_sendfinishcallback& asendfinishfun
		) :
			m_fun(acallfun),
			m_closefun(aclosefun),
			m_sendfinishfun(asendfinishfun),
			m_port(aport),
			m_service_io_(athread, 10240)
		{
			boost::asio::io_service& lioservice = *m_service_io_.get_ioservice(m_service_io_.m_recvthreadsize);
			m_acceptor = new boost::asio::ip::tcp::acceptor(lioservice, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), m_port));
			//m_acceptor = new boost::asio::ip::tcp::acceptor(lioservice, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), m_port));

			accept();			
		}

		impl_asio_ws(
			i32_threadsize athread
			, const ws_callback& acallfun
			, const ws_closecallback& aclosefun
			, const ws_sendfinishcallback& asendfinishfun
		) :
			m_fun(acallfun),
			m_closefun(aclosefun),
			m_sendfinishfun(asendfinishfun),
			m_port(-1),
			m_service_io_(athread + 1, 10240)
		{
		}

		inline void accept()
		{
			service_ws* lservice = nullptr;
			{
				monopoly_shared_lock(m_maplock);
				lservice = new service_ws(m_service_io_, service_io::global_sessionid());
				m_data[lservice->m_sessionid] = lservice;
			}
			m_acceptor->async_accept(
				lservice->socket(),
				boost::bind(
					&impl_asio_ws::handle_accept, this, lservice, boost::asio::placeholders::error)
			);
		}

		inline void handle_accept(service_ws* aservice, const boost::system::error_code& error)
		{
			if (error)
			{
				close(aservice);
				LogLocalError("asio_tcp::handle_accept[%]", error.message().c_str())				
			}
			else
			{
				// Set suggested timeout settings for the websocket
				aservice->m_ws.set_option(boost::beast::websocket::stream_base::timeout{
					std::chrono::seconds(30),
					std::chrono::seconds(600),//boost::beast::websocket::stream_base::none(),
					false });

				// Set a decorator to change the Server of the handshake
				aservice->m_ws.set_option(boost::beast::websocket::stream_base::decorator(
					[](boost::beast::websocket::response_type& res)
					{
						res.set(boost::beast::http::field::server,
						std::string(BOOST_BEAST_VERSION_STRING) +
						" websocket-server-coro");
					}));

				aservice->m_ws.async_accept([this, aservice](::boost::beast::error_code aerr)
					{
						if (aerr)
						{
							LogLocalError("asio_tcp::handle_accept[%]", aerr.message().c_str())
							return;
						}
						{
							monopoly_shared_lock(m_ipportlock);
							std::pair<str_ip, i16_port>& lipport = m_ipport[aservice->m_sessionid];
							lipport.first = aservice->socket().remote_endpoint().address().to_string();
							lipport.second = aservice->socket().remote_endpoint().port();
							aservice->m_is_lanip = tools::is_lanip(lipport.first);
						}
						start(aservice);
					});
			}
			accept();
		}

		inline void close_net(i32_sessionid sessionid)
		{
			service_ws* lpservice = nullptr;
			{
				monopoly_shared_lock(m_maplock);
				std::unordered_map<i32_sessionid, service_ws*>::iterator itor = m_data.find(sessionid);
				if (itor != m_data.end())
				{
					//delete itor->second;
					lpservice = itor->second;
					m_data.erase(itor);
				}
				m_sessionclose.erase(sessionid);
			}

			if (lpservice != nullptr)
			{
				delete lpservice;
			}
		}

		inline void close(i32_sessionid sessionid)
		{
			if (sessionid <= 0)
				return;
			LogLocalError("close sessionid[%]", sessionid)
			// 通知逻辑层session断开连接
			service_ws* lpservice = nullptr;
			std::function<void()> lclosefun = nullptr;
			{
				monopoly_shared_lock(m_maplock);
				auto itor = m_data.find(sessionid);
				if (itor != m_data.end())
				{
					//delete itor->second;
					lpservice = itor->second;
					m_data.erase(itor);
				}

				auto lclosefunitor = m_sessionclose.find(sessionid);
				if (lclosefunitor != m_sessionclose.end())
				{
					lclosefun = lclosefunitor->second;
				}
				m_sessionclose.erase(sessionid);
			}

			if (lpservice != nullptr)
				m_closefun(sessionid);

			if (lclosefun != nullptr)
				lclosefun();

			if (lpservice != nullptr)
			{
				m_fun(lpservice, nullptr, 0);
				delete lpservice;
			}
		}

		inline void close(service_ws* ap)
		{
			LogLocalError("asio_ws::close[%]", ap->m_sessionid)
			close(ap->m_sessionid);
		}

		inline service_ws* connect(
			const str_host& ahost
			, i16_port aport
			, const ws_connectcallback& afun
			, int acount
		)
		{
			LogLocalError("connect host[%] port[%]", ahost, aport)
			service_ws* lservice = nullptr;
			{
				monopoly_shared_lock(m_maplock);
				lservice = new service_ws(m_service_io_, service_io::global_sessionid());
				m_data[lservice->m_sessionid] = lservice;
			}
			auto const address = boost::asio::ip::make_address(ahost);
			boost::asio::ip::tcp::endpoint endpoint(address, boost::lexical_cast<uint16_t>(aport));

			lservice->socket().async_connect(
				endpoint,
				boost::bind(&impl_asio_ws::conn_handler, this, boost::asio::placeholders::error, lservice, ahost, aport, afun, acount)
			);
			//////////////
			return lservice;
		}

		inline void conn_handler(
			const boost::system::error_code& ec
			, service_ws* ap
			, const str_host& ahost
			, i16_port aport
			, const ws_connectcallback& afun
			, int acount)
		{
			if (ec)
			{
				if (acount > 0)
				{
					LogLocalError("连接[%:%]失败[%] 加入定时队列 ", ahost, aport, ec.message())
					//加入定时队列
					wheel_parm lparm
					{
						.m_ms = 1000,
						.m_intervalms = [](int64_t) {return 1000; } ,
						.m_count = 1,
						.m_fun = [this, ahost, aport, afun, acount](wheel_node* anode) {impl_asio_ws::connect(ahost, aport, afun, acount - 1); }
					};
					asio_timer::wheel().addtimer(lparm);
				}
				return;
			}

			// Set suggested timeout settings for the websocket
			ap->m_ws.set_option(boost::beast::websocket::stream_base::timeout{
				std::chrono::seconds(30),
				std::chrono::seconds(600),//boost::beast::websocket::stream_base::none(),
				false });

			// Set a decorator to change the Server of the handshake
			ap->m_ws.set_option(boost::beast::websocket::stream_base::decorator(
				[](boost::beast::websocket::response_type& res)
				{
					res.set(boost::beast::http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
					" websocket-server-coro");
				}));

			ap->m_ws.handshake("0.0.0.0", "/");
			{
				monopoly_shared_lock(m_ipportlock);
				std::pair<str_ip, i16_port>& lipport = m_ipport[ap->m_sessionid];
				lipport.first = ap->socket().remote_endpoint().address().to_string();
				lipport.second = ap->socket().remote_endpoint().port();
				ap->m_is_lanip = tools::is_lanip(lipport.first);
			}

			start(ap);
			afun(ap->m_sessionid);
		}


		template <typename T>
		inline bool spack(i32_sessionid asessionid, std::shared_ptr<T>& apack)
		{
			service_ws* ws = get_ws(asessionid);
			if (ws == nullptr)
				return false;

			std::list<node_pack>* llist = nullptr;
			{
				monopoly_shared_lock(ws->m_mutex);
				ws->m_list.push_back({ asessionid, apack });
				if (ws->m_issend == false)
				{
					llist = new std::list<node_pack>();
					ws->m_list.swap(*llist);
					ws->m_issend = true;
				}
			}
			if(llist != nullptr && llist->empty() == false)
				do_send(ws, llist);
			return true;
		}

		inline bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
		{
			return spack(asessionid, apack);
		}

		inline bool sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack)
		{
			return spack(asessionid, apack);
		}

		inline service_ws* get_ws(i32_sessionid asessionid)
		{
			monopoly_shared_lock(m_maplock);
			std::unordered_map<i32_sessionid, service_ws*>::iterator itor = m_data.find(asessionid);
			if (itor != m_data.end())
			{
				return itor->second;
			}
			return nullptr;
		}

		inline void do_send(service_ws* ws, std::list<node_pack>* alist)
		{
			if (alist->empty())
			{
				{
					monopoly_shared_lock(ws->m_mutex);
					if (ws->m_list.empty() == false)
					{
						ws->m_list.swap(*alist);
					}
					else
					{
						delete alist;
						ws->m_issend = false;
						return;
					}
				}
			}

			node_pack& item = *alist->begin();
			if (ws != nullptr)
			{
				if (item.ispack())
				{//pack
					std::shared_ptr<pack>& lpack = item.get_pack();
					if (lpack->m_pos == lpack->m_len)
					{
						ws->m_ws.async_write(
							boost::asio::buffer(lpack->m_buff, lpack->m_pos),
							[this, alist, ws, lpack](const std::error_code& ec, std::size_t /*length*/)
							{
								alist->pop_front();
								handle_write(ws, ec, lpack);
								if (ec)
								{
									delete alist;
									return;
								}
								do_send(ws, alist);
							}
						);
					}
					else
					{//gateway转发专用
						int lsize = lpack->m_len - lpack->m_pos;
						if (lsize < 0)
							return;
						ws->m_ws.async_write(
							boost::asio::buffer(&lpack->m_buff[lpack->m_pos], lsize),
							[this, alist, ws, lpack](const std::error_code& ec, std::size_t /*length*/)
							{
								alist->pop_front();
								handle_write(ws, ec, lpack);
								if (ec)
								{
									delete alist;
									return;
								}
								do_send(ws, alist);
							}
						);
					}
				}
				else
				{
					std::shared_ptr<void>& lpack = item.get_voidpack();
					pack* lpackptr = (pack*)lpack.get();

					ws->m_ws.async_write(
						boost::asio::buffer(lpackptr->m_buff, lpackptr->m_pos),
						[this, alist, ws, lpack](const std::error_code& ec, std::size_t /*length*/)
						{
							alist->pop_front();
							handle_write_void(ws, ec, lpack);
							if (ec)
							{
								delete alist;
								return;
							}
							do_send(ws, alist);
						}
					);
				}
			}
		}

		inline void handle_write(service_ws* ap, const boost::system::error_code& error, const std::shared_ptr<pack>& apack)
		{
			if (error)
			{
				LogLocalError("asio_ws::handle_write[%]", error.message().c_str())
				close(ap);
			}
			m_sendfinishfun(ap->m_sessionid, error ? true : false, (pack*)apack.get());
		}

		inline void handle_write_void(service_ws* ap, const boost::system::error_code& error, const std::shared_ptr<void>& apack)
		{
			if (error)
			{
				LogLocalError("asio_ws::handle_write[%]", error.message().c_str())
				close(ap);
			}
			m_sendfinishfun(ap->m_sessionid, error ? true : false, (pack*)apack.get());
		}

		inline bool sessionid2ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair)
		{
			monopoly_shared_lock(m_ipportlock);
			std::unordered_map<i32_sessionid, std::pair<str_ip, i16_port>>::iterator itor = m_ipport.find(assionid);
			if (itor == m_ipport.end())
			{
				return false;
			}
			apair = itor->second;
			return true;
		}

		inline bool exist_session(i32_sessionid asession)
		{
			monopoly_shared_lock(m_ipportlock);
			return m_ipport.find(asession) == m_ipport.end();
		}

		inline void handle_read(const boost::system::error_code& error, service_ws* aservice, char* abuff, size_t bytes_transferred)
		{
			if (!error)
			{
				if (!m_fun(aservice, abuff, (uint32_t)bytes_transferred))
					close(aservice);
				else
					start(aservice);
			}
			else
			{
				//关闭连接
				close(aservice);
				LogLocalError("asio_tcp::handle_read[%]", error.message().c_str())
			}
		}

		inline void start(service_ws* aservice)
		{
			std::swap(aservice->m_buff1, aservice->m_buff2);
			aservice->m_ws.async_read_some(boost::asio::buffer(aservice->m_buff1, m_service_io_.m_buffmaxsize),
				boost::bind(&impl_asio_ws::handle_read,
					this,
					boost::asio::placeholders::error,
					aservice,
					aservice->m_buff1,
					boost::asio::placeholders::bytes_transferred)
			);
		}

		inline void set_close(i32_sessionid asession, const std::function<void()>& afun)
		{
			monopoly_shared_lock(m_maplock);
			m_sessionclose[asession] = afun;
		}
	};

	asio_ws::asio_ws(
		i16_port aport
		, i32_threadsize athread
		, const ws_callback& acallfun
		, const ws_closecallback& aclosefun
		, const ws_sendfinishcallback& asendfinishfun
	)
	{
		m_impl_asio_ws.make_unique(aport, athread, acallfun, aclosefun, asendfinishfun);
	}

	asio_ws::asio_ws(
		i32_threadsize athread
		, const ws_callback& acallfun
		, const ws_closecallback& aclosefun
		, const ws_sendfinishcallback& asendfinishfun
	)
	{
		m_impl_asio_ws.make_unique(athread, acallfun, aclosefun, asendfinishfun);
	}

	service_ws* asio_ws::connect(
		const str_host& ahost
		, i16_port aport
		, const ws_connectcallback& afun
		, int acount/* = 5*/
	)
	{
		return m_impl_asio_ws()->connect(ahost, aport, afun, acount);
	}

	bool asio_ws::sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
	{
		return m_impl_asio_ws()->sendpack(asessionid, apack);
	}

	bool asio_ws::sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack)
	{
		return m_impl_asio_ws()->sendpack(asessionid, apack);
	}

	void asio_ws::close(i32_sessionid sessionid)
	{
		return m_impl_asio_ws()->close(sessionid);
	}

	void asio_ws::close(service_ws* ap)
	{
		return m_impl_asio_ws()->close(ap);
	}

	void asio_ws::close_net(i32_sessionid sessionid)
	{
		return m_impl_asio_ws()->close_net(sessionid);
	}

	bool asio_ws::sessionid2ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair)
	{
		return m_impl_asio_ws()->sessionid2ipport(assionid, apair);
	}

	bool asio_ws::exist_session(i32_sessionid asession)
	{
		return m_impl_asio_ws()->exist_session(asession);
	}

	void asio_ws::set_close(i32_sessionid asession, const std::function<void()>& afun)
	{
		return m_impl_asio_ws()->set_close(asession, afun);
	}
}