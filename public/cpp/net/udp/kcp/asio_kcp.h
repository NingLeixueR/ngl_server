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

#include "kcp_endpoint.h"
#include "kcp_session.h"
#include "threadtools.h"
#include "time_wheel.h"
#include "asio_base.h"
#include "udp_cmd.h"
#include "type.h"
#include "pack.h"
#include "nlog.h"
#include "ikcp.h"

#include <asio.hpp>
#include <memory>
#include <map>

namespace ngl
{
	class asio_kcp
	{
		asio_kcp() = delete;
		asio_kcp(const asio_kcp&) = delete;
		asio_kcp& operator=(const asio_kcp&) = delete;

	public:
		enum
		{
			e_buff_byte = 1500,
			e_buffrecv_byte = 10240,
			e_close_intervalms = 1* localtime::MILLISECOND,
			e_waitrecv_intervalms = 1* localtime::MILLISECOND,
		};
	private:
		kcp_session							m_session;
		std::function<void(i32_session)>	m_connectfun = nullptr;
		bpool								m_pool;
		asio::io_context					m_context;
		asio_udp::socket					m_socket;
		asio_udp_endpoint					m_remoteport;
		char								m_buff[e_buff_byte] = { 0 };
		std::size_t							m_bytes_received = 0;
		char								m_buffrecv[e_buffrecv_byte] = { 0 };
		std::function<void(char*, int)>		m_wait = nullptr;
		asio_udp_endpoint					m_waitendpoint;
		i16_port							m_port = 0;
		std::jthread						m_thread;
		nrate								m_rate;
	public:
		explicit asio_kcp(i16_port port);

		~asio_kcp() = default;
	private:
		void func_ecmd_connect()const;

		void func_ecmd_connect_ret()const;

		void func_ecmd_close()const;
	public:
		// # 发送原始udp包
		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		// # 发送原始udp包并等待其返回
		bool sendu_waitrecv(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun);

		// # 通过kcp发送pack
		bool send_server(i32_sessionid asessionid, const std::shared_ptr<pack>& apack);

		// # 通过kcp给所有client发送pack
		bool send_server(const std::shared_ptr<pack>& apack);

		// # 通过kcp给指定area所有client发送pack
		bool sendpackbyarea(i16_area aarea, const std::shared_ptr<pack>& apack);

		// # 通过kcp发送pack
		bool send_server(const asio_udp_endpoint& aendpoint, const std::shared_ptr<pack>& apack);

		// # 通过kcp发送pack
		bool sendpackbyactorid(i64_actorid aactorid, const std::shared_ptr<pack>& apack);

		// # 发起连接
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aactoridserver
			, i64_actorid aactoridclient
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		);

		// # 发起连接
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aactoridserver
			, i64_actorid aactoridclient
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		);

		// # 查找连接关联的actor
		i64_actorid find_server(i32_session asession);
		i64_actorid find_client(i32_session asession);
		bool find_actorid(i32_session asession, i64_actorid& aactoridserver, i64_actorid& aactoridclient);

		// # 根据actorid获取session
		i32_session find_session(i64_actorid aactoridclient);

		// # 关闭连接
		void close(i32_session asession);

		void close_net(i32_session asession);

		// # 重置连接
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aactoridserver, i64_actorid aactoridclient);

		bool sempack(const ptr_se& apstruct, const char* abuff, int abufflen);

		void start();

		bool send(i32_sessionid asessionid, const char* buf, int len);

		int send(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		int sendbuff(i32_session asession, const char* buf, int len);

		int sendbuff(const asio_udp_endpoint& aendpoint, const char* buf, int len);
	};
}// namespace ngl