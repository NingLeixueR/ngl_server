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
#pragma once

#include "threadtools.h"
#include "asio_base.h"
#include "pack.h"
#include "nlog.h"

namespace ngl
{
	using tcp_callback				= std::function<bool(service_tcp*, const char*, uint32_t)>;
	using tcp_closecallback			= std::function<void(i32_sessionid)>;
	using tcp_connectcallback		= std::function<void(i32_sessionid)>;
	using tcp_sendfinishcallback	= std::function<void(i32_sessionid, bool, const pack*)>;

	class asio_tcp
	{
		asio_tcp() = delete;
		asio_tcp(const asio_tcp&) = delete;
		asio_tcp& operator=(const asio_tcp&) = delete;

		std::shared_ptr<asio::ip::tcp::acceptor>	m_acceptor_v4 = nullptr;
		std::shared_ptr<asio::ip::tcp::acceptor>	m_acceptor_v6 = nullptr;
		i16_port					m_port			= 0;
		tcp_callback				m_fun			= nullptr;
		tcp_closecallback			m_closefun		= nullptr;
		tcp_sendfinishcallback		m_sendfinishfun = nullptr;
		i32_sessionid				m_sessionid		= 0;
		std::shared_mutex			m_maplock;
		serviceio_info				m_service_io_;
		std::shared_mutex			m_ipportlock;
		std::unordered_map<i32_sessionid, std::shared_ptr<service_tcp>> m_data;
		std::unordered_map<i32_sessionid, std::pair<str_ip, i16_port>>	m_ipport;
		std::unordered_map<i32_sessionid, std::function<void()>>		m_sessionclose;
	public:
		friend class service_tcp;

		// # 服务器
		asio_tcp(
			i8_sesindex aindex
			, i16_port aport									// 监听端口
			, i32_threadsize athread							// 线程数
			, const tcp_callback& acallfun						// 回调
			, const tcp_closecallback& aclosefun				// 关闭回调
			, const tcp_sendfinishcallback& asendfinishfun		// 发送失败的回调
		);

		// # 客户端
		asio_tcp(
			i8_sesindex aindex
			, i32_threadsize athread							// 线程数
			, const tcp_callback& acallfun						// 回调
			, const tcp_closecallback& aclosefun				// 关闭回调
			, const tcp_sendfinishcallback& asendfinishfun		//发送失败的回调
		);
	private:
		service_tcp* get_tcp(i32_sessionid asessionid);

		template <typename T>
		bool spack(i32_sessionid asessionid, std::shared_ptr<T>& apack);

		template <typename TPACK>
		void async_send(service_tcp* atcp, const std::shared_ptr<std::list<node_pack>>& alist, std::shared_ptr<TPACK>& apack, char* abuff, int32_t abufflen);

		void do_send(service_tcp* atcp, const std::shared_ptr<std::list<node_pack>>& alist);

		void handle_write(service_tcp* atcp, const std::error_code& error, std::shared_ptr<pack> apack);

		void handle_write(service_tcp* atcp, const std::error_code& error, std::shared_ptr<void> apack);

		void close_socket(asio::ip::tcp::socket& socket);

		void accept_handle(bool aisv4, const std::shared_ptr<service_tcp>& aservice, const std::error_code& error);

		void accept(bool aisv4);

		void start(const std::shared_ptr<service_tcp>& aservice);
	public:
		// # 发起连接
		service_tcp* connect(const str_ip& aip, i16_port aport, const tcp_connectcallback& afun, int acount = 5);

		// # 发送pack
		bool send(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool send(i32_sessionid asessionid, std::shared_ptr<void>& apack);

		// # 关闭连接(会通知逻辑层)
		void close(i32_sessionid sessionid);
		void close(service_tcp* atcp);

		// # 关闭连接(不会通知逻辑层)
		void close_net(i32_sessionid sessionid);

		// # 根据session获取ip与端口
		bool get_ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair);

		// # 设置连接关闭回调
		void set_close(i32_sessionid asession, const std::function<void()>& afun);
	};	
}// namespace ngl