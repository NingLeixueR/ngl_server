#pragma once

#include "session_endpoint.h"
#include "session_manage.h"
#include "threadtools.h"
#include "time_wheel.h"
#include "asio_base.h"
#include "udp_cmd.h"
#include "type.h"
#include "pack.h"
#include "nlog.h"
#include "impl.h"
#include "ikcp.h"
#include "impl.h"

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

	private:		
		session_manage						m_session;
		std::function<void(i32_session)>	m_connectfun;
		bpool								m_pool;
		asio::io_context					m_context;
		asio_udp::socket					m_socket;
		asio_udp_endpoint					m_remoteport;
		char								m_buff[1500];
		std::size_t							m_bytes_received;
		char								m_buffrecv[10240];
		std::function<void(char*, int)>		m_wait;
		asio_udp_endpoint					m_waitendpoint;
		i16_port							m_port;
	public:
		explicit asio_kcp(i16_port port);

		~asio_kcp();

	private:
		bool function_econnect(ptr_se& apstruct, i64_actorid aactorid, bool aconnect);

		void function_ecmd_connect()const;

		void function_ecmd_connect_ret()const;

		void function_ecmd_ping()const;

		void function_ecmd_close()const;
	public:

		// # 发送原始udp包
		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		// # 发送原始udp包并等待其返回
		bool sendu_waitrecv(
			const asio_udp_endpoint& aendpoint, 
			const char* buf, 
			int len, 
			const std::function<void(char*, int)>& afun
		);

		// # 通过kcp发送pack
		bool sendpack(i32_sessionid asessionid, const std::shared_ptr<pack>& apack);

		// # 通过kcp发送pack
		bool sendpack(const asio_udp_endpoint& aendpoint, const std::shared_ptr<pack>& apack);

		// # 发起连接
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aactorid
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		);

		// # 发起连接
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aactorid
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		);

		// # 查找连接关联的actor
		i64_actorid find_actorid(i32_session asession);

		// # 关闭连接
		void close(i32_session asession);

		void close_net(i32_session asession);

		// # 重置连接
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport);

		bool sempack(const ptr_se& apstruct, const char* abuff, int abufflen);

		void start();

		bool send(i32_sessionid asessionid, const char* buf, int len);

		int send(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		int sendbuff(i32_session asession, const char* buf, int len);

		int sendbuff(const asio_udp_endpoint& aendpoint, const char* buf, int len);
	};
}// namespace ngl