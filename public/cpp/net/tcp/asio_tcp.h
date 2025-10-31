/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
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

		asio::ip::tcp::acceptor* m_acceptor_v4;
		asio::ip::tcp::acceptor* m_acceptor_v6;
		i16_port														m_port;
		tcp_callback													m_fun;
		tcp_closecallback												m_closefun;
		tcp_sendfinishcallback											m_sendfinishfun;
		std::shared_mutex												m_maplock;
		serviceio_info													m_service_io_;
		std::shared_mutex												m_ipportlock;
		i32_sessionid													m_sessionid;
		std::unordered_map<i32_sessionid, std::shared_ptr<service_tcp>> m_data;
		std::unordered_map<i32_sessionid, std::pair<str_ip, i16_port>>	m_ipport;
		std::unordered_map<i32_sessionid, std::function<void()>>		m_sessionclose;
	public:
		friend class service_tcp;

		// # ������
		asio_tcp(
			i8_sesindex aindex
			, i16_port aport									// �����˿�
			, i32_threadsize athread							// �߳���
			, const tcp_callback& acallfun						// �ص�
			, const tcp_closecallback& aclosefun				// �رջص�
			, const tcp_sendfinishcallback& asendfinishfun		// ����ʧ�ܵĻص�
		);

		// # �ͻ���
		asio_tcp(
			i8_sesindex aindex
			, i32_threadsize athread							// �߳���
			, const tcp_callback& acallfun						// �ص�
			, const tcp_closecallback& aclosefun				// �رջص�
			, const tcp_sendfinishcallback& asendfinishfun		//����ʧ�ܵĻص�
		);

	private:
		service_tcp* get_tcp(i32_sessionid asessionid);

		template <typename T>
		bool spack(i32_sessionid asessionid, std::shared_ptr<T>& apack);

		template <typename TPACK>
		void async_send(service_tcp* tcp, const std::shared_ptr<std::list<node_pack>>& alist, std::shared_ptr<TPACK>& apack, char* abuff, int32_t abufflen);

		void do_send(service_tcp* tcp, const std::shared_ptr<std::list<node_pack>>& alist);

		void handle_write(service_tcp* ap, const std::error_code& error, std::shared_ptr<pack> apack);

		void handle_write(service_tcp* ap, const std::error_code& error, std::shared_ptr<void> apack);

		void close_socket(asio::ip::tcp::socket& socket);

		void accept_handle(bool aisv4, const std::shared_ptr<service_tcp>& aservice, const std::error_code& error);

		void accept(bool aisv4);

		void start(const std::shared_ptr<service_tcp>& aservice);
	public:
		// # ��������
		service_tcp* connect(
			const str_ip& aip							// ip
			, i16_port aport							// �˿�
			, const tcp_connectcallback& afun			// ���ӻص�
			, int acount = 5							// ����ʧ���Զ���������
		);

		// # ����pack
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack);

		// # �ر�����(��֪ͨ�߼���)
		void close(i32_sessionid sessionid);
		void close(service_tcp* ap);

		// # �ر�����(����֪ͨ�߼���)
		void close_net(i32_sessionid sessionid);

		// # ����session��ȡip��˿�
		bool get_ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair);

		// # ��ѯsession��Ӧ�������Ƿ����
		bool exist_session(i32_sessionid asession);

		// # �������ӹرջص�
		void set_close(i32_sessionid asession, const std::function<void()>& afun);
	};	
}// namespace ngl