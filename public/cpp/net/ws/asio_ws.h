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

#include "tools/serialize/pack.h"
#include "tools/threadtools.h"
#include "tools/log/nlog.h"
#include "net/asio_base.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace ngl
{
	struct ws_tls_options
	{
		std::string m_certificate_chain;
		std::string m_private_key;
		std::string m_ca_certificates;
		bool m_verify_peer = true;
	};

	using ws_callback = std::function<bool(service_ws*, const char*, uint32_t)>;
	using ws_closecallback = std::function<void(i32_sessionid)>;
	using ws_connectcallback = std::function<void(i32_sessionid)>;
	using ws_sendfinishcallback = std::function<void(i32_sessionid, bool, const pack*)>;

	class asio_ws
	{
		asio_ws() = delete;
		asio_ws(const asio_ws&) = delete;
		asio_ws& operator=(const asio_ws&) = delete;

		using map_service_ws = std::unordered_map<i32_sessionid, std::shared_ptr<service_ws>>;
		using map_ipport = std::unordered_map<i32_sessionid, std::pair<str_ip, i16_port>>;
		using map_close = std::unordered_map<i32_sessionid, std::function<void()>>;

		std::shared_ptr<basio_tcpacceptor> m_acceptor = nullptr;
		i16_port m_port = 0;
		bool m_use_tls = false;
		ws_callback m_fun = nullptr;
		ws_closecallback m_closefun = nullptr;
		ws_sendfinishcallback m_sendfinishfun = nullptr;
		i32_sessionid m_sessionid = 0;
		serviceio_info m_service_ios;
		std::shared_mutex m_maplock;
		std::shared_mutex m_ipportlock;
		map_service_ws m_data;
		map_ipport m_ipport;
		map_close m_close;
		std::unique_ptr<basio_sslcontext> m_tls_context = nullptr;
		ws_tls_options m_tls_options;
	public:
		enum
		{
			ews_connect_interval = 1,
			ews_default_recvbuff = 20480,
		};

		asio_ws(
			i16_port aport,
			i32_threadsize athread,
			bool ause_tls,
			const ws_callback& acallfun,
			const ws_closecallback& aclosefun,
			const ws_sendfinishcallback& asendfinishfun,
			const ws_tls_options& atls_options = {}
		);

		asio_ws(
			i32_threadsize athread,
			bool ause_tls,
			const ws_callback& acallfun,
			const ws_closecallback& aclosefun,
			const ws_sendfinishcallback& asendfinishfun,
			const ws_tls_options& atls_options = {}
		);

		~asio_ws();
	private:
		std::shared_ptr<service_ws> create_service();
		std::shared_ptr<service_ws> get_ws(i32_sessionid asessionid);
		void prepare_tls_context(bool aserver);
		void configure_websocket(const std::shared_ptr<service_ws>& aservice, bool aserver);
		bool cache_remote_endpoint(const std::shared_ptr<service_ws>& aservice);
		void handle_connect_failure(
			const std::shared_ptr<service_ws>& aservice,
			const basio_errorcode& aerror,
			const str_host& ahost,
			i16_port aport,
			const std::string& atarget,
			const ws_connectcallback& afun,
			int acount,
			const char* astage
		);
		void begin_server_handshake(const std::shared_ptr<service_ws>& aservice);
		void begin_client_handshake(
			const std::shared_ptr<service_ws>& aservice,
			const str_host& ahost,
			const std::string& atarget,
			const ws_connectcallback& afun,
			i16_port aport,
			int acount
		);
		bool queue_send(i32_sessionid asessionid, const ws_send_node& anode);
		void do_send(const std::shared_ptr<service_ws>& aservice, const std::shared_ptr<std::list<ws_send_node>>& alist);
		void handle_write(const std::shared_ptr<service_ws>& aservice, const basio_errorcode& error, const ws_send_node& anode);
		void accept_handle(const std::shared_ptr<service_ws>& aservice, const basio_errorcode& error);
		void accept();
		void start(const std::shared_ptr<service_ws>& aservice);
		void close_socket(basio_iptcpsocket& asocket);
	public:
		i16_port port() const;

		service_ws* connect(const str_host& ahost, i16_port aport, const std::string& atarget, const ws_connectcallback& afun, int acount = 5);
		service_ws* connect(const str_host& ahost, i16_port aport, const ws_connectcallback& afun, int acount = 5);

		bool send(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool send(i32_sessionid asessionid, std::shared_ptr<void>& apack);
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack);
		bool send_msg(i32_sessionid asessionid, const std::string& amsg);

		void close(i32_sessionid sessionid);
		void close(service_ws* asession);
		void close_net(i32_sessionid sessionid);

		bool get_ipport(i32_sessionid asessionid, std::pair<str_ip, i16_port>& apair);
		bool sessionid2ipport(i32_sessionid asessionid, std::pair<str_ip, i16_port>& apair);
		bool exist_session(i32_sessionid asessionid);

		void set_close(i32_sessionid asession, const std::function<void()>& afun);
	};
}// namespace ngl
