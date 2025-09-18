#pragma once

#include "session_endpoint.h"
#include "threadtools.h"
#include "time_wheel.h"
#include "asio_base.h"
#include "type.h"
#include "pack.h"
#include "nlog.h"
#include "ikcp.h"

#include <asio.hpp>
#include <memory>
#include <map>

namespace ngl
{
	using ptr_se = std::shared_ptr<session_endpoint>;

	class session_manage
	{
		session_manage() = delete;
		session_manage(const session_manage&) = delete;
		session_manage& operator=(const session_manage&) = delete;

		std::map<i32_sessionid, ptr_se>						m_dataofsession;
		std::map<std::string, std::map<i16_port, ptr_se>>	m_dataofendpoint;
		int32_t												m_sessionid;
		std::shared_mutex									m_mutex;
		asio_kcp*											m_asiokcp;
	public:
		session_manage(asio_kcp* asiokcp);

		ptr_se add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactorid);

		ptr_se reset_add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactorid);

		void erase(const asio_udp_endpoint& aendpoint);

		void erase(i32_sessionid asession);
	private:
		ptr_se _find(i32_sessionid asession);

		ptr_se _find(const asio_udp_endpoint& aendpoint);
	public:
		ptr_se find(i32_sessionid asession);

		ptr_se find(const asio_udp_endpoint& aendpoint);

		asio_udp_endpoint* find_endpoint(i32_sessionid asession);
	};
}//namespace ngl