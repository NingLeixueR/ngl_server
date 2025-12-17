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
#include "asio_timer.h"
#include "nprotocol.h"
#include "asio_tcp.h"
#include "nlog.h"
#include "pack.h"

namespace ngl
{
	asio_tcp::asio_tcp(i8_sesindex aindex, i16_port aport, i32_threadsize athread, const tcp_callback& acallfun, const tcp_closecallback& aclosefun, const tcp_sendfinishcallback& asendfinishfun) :
		m_fun(acallfun),
		m_closefun(aclosefun),
		m_sendfinishfun(asendfinishfun),
		m_port(aport),
		m_service_io_(athread, 10240),
		m_sessionid(aindex << 24),
		m_acceptor_v4(nullptr),
		m_acceptor_v6(nullptr)
	{
		asio::io_service& lioservice = *m_service_io_.get_ioservice(m_service_io_.m_recvthreadsize);

		m_acceptor_v4 = std::make_shared<asio::ip::tcp::acceptor>(lioservice, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), m_port));
		m_acceptor_v6 = std::make_shared<asio::ip::tcp::acceptor>(lioservice, asio::ip::tcp::endpoint(asio::ip::tcp::v6(), m_port));
		m_acceptor_v4->set_option(asio::socket_base::reuse_address(true));
		m_acceptor_v6->set_option(asio::socket_base::reuse_address(true));
		accept(true);
		accept(false);
	}

	asio_tcp::asio_tcp(i8_sesindex aindex, i32_threadsize athread, const tcp_callback& acallfun, const tcp_closecallback& aclosefun, const tcp_sendfinishcallback& asendfinishfun) :
		m_fun(acallfun),
		m_closefun(aclosefun),
		m_sendfinishfun(asendfinishfun),
		m_port(-1),
		m_service_io_(athread + 1, 10240),
		m_sessionid(aindex << 24),
		m_acceptor_v4(nullptr),
		m_acceptor_v6(nullptr)
	{
	}

	service_tcp* asio_tcp::connect(const str_ip& aip, i16_port aport, const tcp_connectcallback& afun, int acount)
	{
		//log_error()->print("connect {}:{}", aip, aport);
		std::shared_ptr<service_tcp> lservice = nullptr;
		{
			monopoly_shared_lock(m_maplock);
			lservice = std::make_shared<service_tcp>(m_service_io_, ++m_sessionid);
			m_data[lservice->m_sessionid] = lservice;
		}
		lservice->m_socket.async_connect(basio_iptcpendpoint(basio_ipaddress::from_string(aip), aport), 
			[this, lservice, aip, aport, afun, acount](const std::error_code& ec)
			{
				if (ec)
				{
					if (acount > 0)
					{
						log_error()->print("connect [{}:{}] fail [{}] add timer list! ", aip, aport, ec.message());
						// 加入定时队列
						wheel_parm lparm
						{
							.m_ms = 1 * localtime::MILLISECOND,
							.m_intervalms = [](int64_t) {return 1 * localtime::MILLISECOND; } ,
							.m_count = 1,
							.m_fun = [this, aip, aport, afun, acount](const wheel_node* anode)
							{
								connect(aip, aport, afun, acount - 1);
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
		return lservice.get();
	}

	service_tcp* asio_tcp::get_tcp(i32_sessionid asessionid)
	{
		monopoly_shared_lock(m_maplock);
		std::shared_ptr<service_tcp>* lp = tools::findmap(m_data, asessionid);
		return lp == nullptr ? nullptr : lp->get();
	}

	template <typename T>
	bool asio_tcp::spack(i32_sessionid asessionid, std::shared_ptr<T>& apack)
	{
		service_tcp* tcp = get_tcp(asessionid);
		if (tcp == nullptr)
		{
			return false;
		}
		std::shared_ptr<std::list<node_pack>> llist = nullptr;
		{
			monopoly_shared_lock(tcp->m_mutex);
			tcp->m_list.push_back({ asessionid, apack });
			if (tcp->m_issend == false)
			{
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

	bool asio_tcp::sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack)
	{
		return spack(asessionid, apack);
	}

	bool asio_tcp::sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack)
	{
		return spack(asessionid, apack);
	}

	template <typename TPACK>
	void asio_tcp::async_send(service_tcp* tcp, const std::shared_ptr<std::list<node_pack>>& alist, std::shared_ptr<TPACK>& apack, char* abuff, int32_t abufflen)
	{
		tcp->m_socket.async_send(asio::buffer(abuff, abufflen)
			, [this, alist, tcp, apack](const std::error_code& ec, std::size_t /*length*/)
			{
				alist->pop_front();
				handle_write(tcp, ec, apack);
				if (ec)
				{
					log_error()->print("asio_tcp::do_send fail [{}]", ec.message().c_str());
					return;
				}
				do_send(tcp, alist);
			}
		);
	}

	void asio_tcp::do_send(service_tcp* tcp, const std::shared_ptr<std::list<node_pack>>& alist)
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
					return;
				}
				async_send(tcp, alist, lpack, &lpack->m_buff[lpos], lsize);
			}
			else
			{
				std::shared_ptr<void>& lpack = item.get_voidpack();
				pack* lpackptr = (pack*)lpack.get();
				async_send(tcp, alist, lpack, lpackptr->m_buff, lpackptr->m_pos);
			}
		}
	}

	void asio_tcp::handle_write(service_tcp* ap, const std::error_code& error, std::shared_ptr<pack> apack)
	{
		if (error)
		{
			log_error()->print("asio_tcp::handle_write[{}]", error.message().c_str());
			close(ap);
		}
		m_sendfinishfun(ap->m_sessionid, error ? true : false, apack.get());
	}

	void asio_tcp::handle_write(service_tcp* ap, const std::error_code& error, std::shared_ptr<void> apack)
	{
		if (error)
		{
			log_error()->print("asio_tcp::handle_write[{}]", error.message().c_str());
			close(ap);
		}
		m_sendfinishfun(ap->m_sessionid, error ? true : false, (pack*)apack.get());
	}

	void asio_tcp::close(i32_sessionid sessionid)
	{
		if (sessionid <= 0)
		{
			return;
		}

		// 通知逻辑层session断开连接
		std::shared_ptr<service_tcp> lpservice = nullptr;
		std::function<void()> lclosefun = nullptr;
		{
			monopoly_shared_lock(m_maplock);
			auto itor = m_data.find(sessionid);
			if (itor != m_data.end())
			{
				lpservice = itor->second;
				m_data.erase(itor);
			}
			log_error()->print("asio_tcp close sessionid [{}]", sessionid);

			auto lclosefunitor = m_sessionclose.find(sessionid);
			if (lclosefunitor != m_sessionclose.end())
			{
				lclosefun = lclosefunitor->second;
			}
			m_sessionclose.erase(sessionid);
		}

		if (lpservice != nullptr)
		{
			m_closefun(sessionid);
		}

		if (lclosefun != nullptr)
		{
			lclosefun();
		}

		if (lpservice != nullptr)
		{
			m_fun(lpservice.get(), nullptr, 0);
		}
	}

	void asio_tcp::close(service_tcp* ap)
	{
		close(ap->m_sessionid);
		close_socket(ap->m_socket);
	}

	void  asio_tcp::close_socket(asio::ip::tcp::socket& socket)
	{
		asio::error_code ec;

		// 步骤1: 取消所有异步操作
		socket.cancel(ec);
		if (ec)
		{
			std::cerr << "Cancel error: " << ec.message() << "\n";
		}

		// 步骤2: 关闭连接方向（可选但推荐）
		if (socket.is_open())
		{
			socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
			// 忽略"not_connected"错误（可能已自然关闭）
			if (ec && ec != asio::error::not_connected)
			{
				std::cerr << "Shutdown error: " << ec.message() << "\n";
			}
		}

		// 步骤3: 关闭socket释放资源
		if (socket.is_open())
		{
			socket.close(ec);
			if (ec) 
			{
				std::cerr << "Close error: " << ec.message() << "\n";
			}
		}
	}

	void asio_tcp::close_net(i32_sessionid sessionid)
	{
		std::shared_ptr<service_tcp> lpservice = nullptr;
		{
			monopoly_shared_lock(m_maplock);
			auto itor = m_data.find(sessionid);
			if (itor != m_data.end())
			{
				lpservice = itor->second;
				m_data.erase(itor);
			}
			m_sessionclose.erase(sessionid);
		}
		if (lpservice != nullptr)
		{
			close_socket(lpservice->m_socket);
		}
	}

	bool asio_tcp::get_ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair)
	{
		monopoly_shared_lock(m_ipportlock);
		auto lpair = tools::findmap(m_ipport, assionid);
		if (lpair == nullptr)
		{
			return false;
		}
		apair = *lpair;
		return true;
	}

	bool asio_tcp::exist_session(i32_sessionid asession)
	{
		monopoly_shared_lock(m_ipportlock);
		return m_ipport.find(asession) != m_ipport.end();
	}

	void  asio_tcp::accept_handle(bool aisv4, const std::shared_ptr<service_tcp>& aservice, const std::error_code& error)
	{
		if (error)
		{
			close(aservice.get());
			log_error()->print("asio_tcp::accept[{}]", error.message().c_str());
		}
		else
		{
			{
				monopoly_shared_lock(m_ipportlock);
				std::pair<str_ip, i16_port>& lipport = m_ipport[aservice->m_sessionid];
				lipport.first = aservice->m_socket.remote_endpoint().address().to_string();
				lipport.second = aservice->m_socket.remote_endpoint().port();
				aservice->m_is_lanip = tools::is_lanip(lipport.first);
			}
			start(aservice);
		}
		accept(true);
	}

	void  asio_tcp::accept(bool aisv4)
	{
		std::shared_ptr<service_tcp> lservice = nullptr;
		{
			monopoly_shared_lock(m_maplock);
			lservice = std::make_shared<service_tcp>(m_service_io_, ++m_sessionid);
			m_data[lservice->m_sessionid] = lservice;
		}
		if (aisv4)
		{
			m_acceptor_v4->async_accept(lservice->m_socket, [this, lservice](const std::error_code& error)
				{
					accept_handle(true, lservice, error);
				}
			);
		}
		else
		{
			m_acceptor_v6->async_accept(lservice->m_socket, [this, lservice](const std::error_code& error)
				{
					accept_handle(false, lservice, error);
				}
			);
		}
	}

	void  asio_tcp::start(const std::shared_ptr<service_tcp>& aservice)
	{
		std::swap(aservice->m_buff1, aservice->m_buff2);
		char* lbuff = aservice->m_buff1;
		aservice->m_socket.async_read_some(asio::buffer(aservice->m_buff1, m_service_io_.m_buffmaxsize)
			, [this, lbuff, aservice](const std::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					if (!m_fun(aservice.get(), lbuff, (uint32_t)bytes_transferred))
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
					//关闭连接
					close(aservice.get());
					log_error()->print("asio_tcp::handle_read[{}]", error.message().c_str());
				}
			}
		);
	}

	void asio_tcp::set_close(i32_sessionid asession, const std::function<void()>& afun)
	{
		monopoly_shared_lock(m_maplock);
		m_sessionclose[asession] = afun;
	}
}// namespace ngl