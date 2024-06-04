#pragma once

#include "threadtools.h"
#include "time_wheel.h"
#include "json_write.h"
#include "json_read.h"
#include "asio_base.h"
#include "splite.h"
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
	using asio_udp			= asio::ip::udp;
	using asio_udp_endpoint = asio_udp::endpoint;

	class asio_kcp;

	extern time_wheel m_kcptimer;

	class asio_kcp
	{
	public:
		struct impl_asio_kcp;
	private:		
		ngl::impl<impl_asio_kcp> m_impl_asio_kcp;
	public:
		asio_kcp(i16_port port);

		~asio_kcp();

		// # 发送原始udp包
		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		// # 发送原始udp包并等待其返回
		bool sendu_waitrecv(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun);

		// # 通过kcp发送pack
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack);

		// # 通过kcp发送pack
		bool sendpack(const asio_udp_endpoint& aendpoint, std::shared_ptr<pack>& apack);

		// # 发起连接
		void connect(int32_t aconv
			, const std::string& akcpsess
			, i64_actorid aactorid
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		);

		// # 发起连接
		void connect(int32_t aconv
			, const std::string& akcpsess
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

		impl_asio_kcp* get_impl();	
	};
}// namespace ngl