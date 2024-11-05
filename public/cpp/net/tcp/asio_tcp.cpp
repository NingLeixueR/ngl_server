#include "asio_timer.h"
#include "nprotocol.h"
#include "asio_tcp.h"
#include "nlog.h"
#include "pack.h"

namespace ngl
{
	struct asio_tcp::impl_asio_tcp
	{
		impl_asio_tcp() = delete;
		impl_asio_tcp(const impl_asio_tcp&) = delete;
		impl_asio_tcp& operator=(const impl_asio_tcp&) = delete;

		asio::ip::tcp::acceptor*										m_acceptor;
		i16_port														m_port;
		tcp_callback													m_fun;
		tcp_closecallback												m_closefun;
		tcp_sendfinishcallback											m_sendfinishfun;
		std::shared_mutex												m_maplock;
		serviceio_info													m_service_io_;
		std::shared_mutex												m_ipportlock;
		int32_t															m_sessionid;
		std::unordered_map<i32_sessionid, service_tcp*>					m_data;
		std::unordered_map<i32_sessionid, std::pair<str_ip, i16_port>>	m_ipport;
		std::unordered_map<i32_sessionid, std::function<void()>>		m_sessionclose;

		impl_asio_tcp(
			i8_sesindex aindex
			, i16_port aport
			, i32_threadsize athread
			, const tcp_callback& acallfun
			, const tcp_closecallback& aclosefun
			, const tcp_sendfinishcallback& asendfinishfun
		) :
			m_fun(acallfun),
			m_closefun(aclosefun),
			m_sendfinishfun(asendfinishfun),
			m_port(aport),
			m_service_io_(athread, 10240),
			m_sessionid(aindex << 24),
			m_acceptor(nullptr)
		{
			asio::io_service& lioservice = *m_service_io_.get_ioservice(m_service_io_.m_recvthreadsize);
			m_acceptor = new asio::ip::tcp::acceptor(
				lioservice, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), m_port)
			);
			// m_acceptor = new boost::asio::ip::tcp::acceptor(
			//  lioservice, 
			//  boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), m_port)
			// );
			accept();
		}

		impl_asio_tcp(
			i8_sesindex aindex
			, i32_threadsize athread
			, const tcp_callback& acallfun
			, const tcp_closecallback& aclosefun
			, const tcp_sendfinishcallback& asendfinishfun
		) :
			m_fun(acallfun),
			m_closefun(aclosefun),
			m_sendfinishfun(asendfinishfun),
			m_port(-1),
			m_service_io_(athread + 1, 10240),
			m_sessionid(aindex << 24),
			m_acceptor(nullptr)
		{
		}

		inline service_tcp* connect(
			const str_ip& aip
			, i16_port aport
			, const tcp_connectcallback& afun
			, int acount
		)
		{
			log_error()->print("connect ip:port {}:{}", aip, aport);
			service_tcp* lservice = nullptr;
			{
				monopoly_shared_lock(m_maplock);
				lservice = new service_tcp(m_service_io_, ++m_sessionid);
				m_data[lservice->m_sessionid] = lservice;
			}
			lservice->m_socket.async_connect(
				basio_iptcpendpoint(basio_ipaddress::from_string(aip), aport),
				[this, lservice, aip, aport, afun, acount](const std::error_code& ec)
				{
					if (ec)
					{
						if (acount > 0)
						{
							log_error()->print("����[{}:{}]ʧ��[{}] ���붨ʱ���� ", aip, aport, ec.message());
							// ���붨ʱ����
							wheel_parm lparm
							{
								.m_ms = 1000,
								.m_intervalms = [](int64_t) {return 1000; } ,
								.m_count = 1,
								.m_fun = [this, aip, aport, afun, acount](const wheel_node* anode)
								{
									impl_asio_tcp::connect(aip, aport, afun, acount - 1);
								}
							};
							asio_timer::wheel().addtimer(lparm);
						}
						return;
					}
					{
						monopoly_shared_lock(m_ipportlock);
						std::pair<str_ip, i16_port>& lipport = m_ipport[lservice->m_sessionid];
						lipport.first = lservice->m_socket.remote_endpoint().address().to_string();
						lipport.second = lservice->m_socket.remote_endpoint().port();
						lservice->m_is_lanip = tools::is_lanip(lipport.first);
					}
					start(lservice);
					afun(lservice->m_sessionid);
				}
			);
			return lservice;
		}

		inline service_tcp* get_tcp(i32_sessionid asessionid)
		{
			monopoly_shared_lock(m_maplock);
			service_tcp** lp = tools::findmap(m_data, asessionid);
			return lp == nullptr ? nullptr : *lp;
		}

		template <typename T>
		inline bool spack(i32_sessionid asessionid, std::shared_ptr<T>& apack)
		{
			service_tcp* tcp = get_tcp(asessionid);
			if (tcp == nullptr)
				return false;

			std::list<node_pack>* llist = nullptr;
			{
				monopoly_shared_lock(tcp->m_mutex);
				tcp->m_list.push_back({ asessionid, apack });
				if (tcp->m_issend == false)
				{
					llist = new std::list<node_pack>();
					tcp->m_list.swap(*llist);
					tcp->m_issend = true;
				}
			}
			if (llist != nullptr && llist->empty() == false)
				do_send(tcp, llist);
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

		inline void do_send(service_tcp* tcp, std::list<node_pack>* alist)
		{
			if (alist->empty())
			{
				{
					monopoly_shared_lock(tcp->m_mutex);
					if (tcp->m_list.empty() == false)
					{
						tcp->m_list.swap(*alist);
					}
					else
					{
						delete alist;
						tcp->m_issend = false;
						return;
					}
				}
			}

			node_pack& item = *alist->begin();
			if (tcp != nullptr)
			{
				if (item.ispack())
				{//pack
					std::shared_ptr<pack>& lpack = item.get_pack();
					if (lpack->m_pos == lpack->m_len)
					{
						tcp->m_socket.async_send(
							asio::buffer(lpack->m_buff, lpack->m_pos),
							[this, alist, tcp, lpack](const std::error_code& ec, std::size_t /*length*/)
							{
								alist->pop_front();
								handle_write(tcp, ec, lpack);
								if (ec)
								{
									delete alist;
									return;
								}
								do_send(tcp, alist);
							});
					}
					else
					{//gatewayת��ר��
						int lsize = lpack->m_len - lpack->m_pos;
						if (lsize < 0)
							return;
						tcp->m_socket.async_send(
							asio::buffer(&lpack->m_buff[lpack->m_pos], lsize),
							[this, alist, tcp, lpack](const std::error_code& ec, std::size_t /*length*/)
							{
								alist->pop_front();
								handle_write(tcp, ec, lpack);
								if (ec)
								{
									delete alist;
									return;
								}
								do_send(tcp, alist);
							}
						);
					}
				}
				else
				{
					std::shared_ptr<void>& lpack = item.get_voidpack();
					pack* lpackptr = (pack*)lpack.get();

					tcp->m_socket.async_send(
						asio::buffer(lpackptr->m_buff, lpackptr->m_pos),
						[this, alist, tcp, lpack](const std::error_code& ec, std::size_t /*length*/)
						{
							alist->pop_front();
							handle_write_void(tcp, ec, lpack);
							if (ec)
							{
								delete alist;
								return;
							}
							do_send(tcp, alist);
						}
					);
				}
			}
		}

		inline void handle_write(
			service_tcp* ap, const std::error_code& error, std::shared_ptr<pack> apack
		)
		{
			if (error)
			{
				log_error()->print("asio_tcp::handle_write[{}]", error.message().c_str());
				close(ap);
			}
			m_sendfinishfun(ap->m_sessionid, error ? true : false, apack.get());
		}

		inline void handle_write_void(
			service_tcp* ap, const std::error_code& error, std::shared_ptr<void> apack
		)
		{
			if (error)
			{
				log_error()->print("asio_tcp::handle_write[{}]", error.message().c_str());
				close(ap);
			}
			m_sendfinishfun(ap->m_sessionid, error ? true : false, (pack*)apack.get());
		}

		inline void close(i32_sessionid sessionid)
		{
			if (sessionid <= 0)
				return;
			
			// ֪ͨ�߼���session�Ͽ�����
			service_tcp* lpservice = nullptr;
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
				log_error()->print("close sessionid[{}]", sessionid);

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

		inline void close(service_tcp* ap)
		{
			log_error()->print("asio_tcp::close[{}]", ap->m_sessionid);
			close(ap->m_sessionid);
		}

		inline void close_net(i32_sessionid sessionid)
		{
			service_tcp* lpservice = nullptr;
			{
				monopoly_shared_lock(m_maplock);
				std::unordered_map<i32_sessionid, service_tcp*>::iterator itor = m_data.find(sessionid);
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

		inline bool get_ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair)
		{
			monopoly_shared_lock(m_ipportlock);
			auto itor = m_ipport.find(assionid);
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
			return m_ipport.find(asession) != m_ipport.end();
		}

		inline void accept()
		{
			service_tcp* lservice = nullptr;
			{
				monopoly_shared_lock(m_maplock);
				lservice = new service_tcp(m_service_io_, ++m_sessionid);
				m_data[lservice->m_sessionid] = lservice;
			}
			m_acceptor->async_accept(
				lservice->m_socket,
				[this,lservice](const std::error_code& error)
				{
					if (error)
					{
						close(lservice);
						log_error()->print("asio_tcp::handle_accept[{}]", error.message().c_str());
					}
					else
					{
						{
							monopoly_shared_lock(m_ipportlock);
							std::pair<str_ip, i16_port>& lipport = m_ipport[lservice->m_sessionid];
							lipport.first = lservice->m_socket.remote_endpoint().address().to_string();
							lipport.second = lservice->m_socket.remote_endpoint().port();
							lservice->m_is_lanip = tools::is_lanip(lipport.first);
						}
						start(lservice);
					}
					accept();
				}
			);
		}

		inline void start(service_tcp* aservice)
		{
			std::swap(aservice->m_buff1, aservice->m_buff2);
			char* lbuff = aservice->m_buff1;
			aservice->m_socket.async_read_some(
				asio::buffer(aservice->m_buff1, m_service_io_.m_buffmaxsize),
				[this, lbuff, aservice](const std::error_code& error, size_t bytes_transferred)
				{
					if (!error)
					{
						if (!m_fun(aservice, lbuff, (uint32_t)bytes_transferred))
							close(aservice);
						else
							start(aservice);
					}
					else
					{
						//�ر�����
						close(aservice);
						log_error()->print("asio_tcp::handle_read[{}]", error.message().c_str());
					}
				}
			);
		}

		inline void set_close(i32_sessionid asession, const std::function<void()>& afun)
		{
			monopoly_shared_lock(m_maplock);
			m_sessionclose[asession] = afun;
		}
	};

	asio_tcp::asio_tcp(
		i8_sesindex aindex, i16_port aport, i32_threadsize athread, 
		const tcp_callback& acallfun, 
		const tcp_closecallback& aclosefun, 
		const tcp_sendfinishcallback& asendfinishfun
	)
	{
		m_impl_asio_tcp.make_unique(aindex, aport, athread, acallfun, aclosefun, asendfinishfun);
	}

	asio_tcp::asio_tcp(
		i8_sesindex aindex, 
		i32_threadsize athread, 
		const tcp_callback& acallfun, 
		const tcp_closecallback& aclosefun, 
		const tcp_sendfinishcallback& asendfinishfun
	)
	{
		m_impl_asio_tcp.make_unique(aindex, athread, acallfun, aclosefun, asendfinishfun);
	}

	service_tcp* asio_tcp::connect(
		const str_ip& aip, 
		i16_port aport, 
		const tcp_connectcallback& afun, 
		int acount/* = 5*/
	)
	{
		return m_impl_asio_tcp()->connect(aip, aport, afun, acount);
	}

	bool asio_tcp::sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
	{
		return m_impl_asio_tcp()->sendpack(asessionid, apack);
	}

	bool asio_tcp::sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack)
	{
		return m_impl_asio_tcp()->sendpack(asessionid, apack);
	}

	void asio_tcp::close(i32_sessionid sessionid)
	{
		m_impl_asio_tcp()->close(sessionid);
	}

	void asio_tcp::close(service_tcp* ap)
	{
		m_impl_asio_tcp()->close(ap);
	}

	void asio_tcp::close_net(i32_sessionid sessionid)
	{
		m_impl_asio_tcp()->close_net(sessionid);
	}

	bool asio_tcp::get_ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair)
	{
		return m_impl_asio_tcp()->get_ipport(assionid, apair);
	}

	bool asio_tcp::exist_session(i32_sessionid asession)
	{
		return m_impl_asio_tcp()->exist_session(asession);
	}

	void asio_tcp::set_close(i32_sessionid asession, const std::function<void()>& afun)
	{
		m_impl_asio_tcp()->set_close(asession, afun);
	}
}// namespace ngl

