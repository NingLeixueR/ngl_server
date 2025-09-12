#pragma once

#include "threadtools.h"
#include "time_wheel.h"
#include "asio_base.h"
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
		asio_kcp() = delete;
		asio_kcp(const asio_kcp&) = delete;
		asio_kcp& operator=(const asio_kcp&) = delete;

	public:
		struct impl_asio_kcp;
	private:		
		ngl::impl<impl_asio_kcp> m_impl_asio_kcp;
	public:
		explicit asio_kcp(i16_port port);

		~asio_kcp();

		// # ����ԭʼudp��
		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		// # ����ԭʼudp�����ȴ��䷵��
		bool sendu_waitrecv(
			const asio_udp_endpoint& aendpoint, 
			const char* buf, 
			int len, 
			const std::function<void(char*, int)>& afun
		);

		// # ͨ��kcp����pack
		bool sendpack(i32_sessionid asessionid, const std::shared_ptr<pack>& apack);

		// # ͨ��kcp����pack
		bool sendpack(const asio_udp_endpoint& aendpoint, const std::shared_ptr<pack>& apack);

		// # ��������
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aactorid
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		);

		// # ��������
		void connect(int32_t aconv
			, std::string& akcpsess
			, i64_actorid aactorid
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		);

		// # �������ӹ�����actor
		i64_actorid find_actorid(i32_session asession);

		// # �ر�����
		void close(i32_session asession);

		void close_net(i32_session asession);

		// # ��������
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport);

		impl_asio_kcp* get_impl();	
	};
}// namespace ngl