/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: KCP session that wraps ikcp state and manages retransmission timers.

#pragma once

#include "net/udp/kcp/kcp_endpoint.h"
#include "tools/serialize/pack.h"
#include "tools/tools/tools_thread.h"
#include "tools/tools/tools_time_wheel.h"
#include "tools/log/nlog.h"
#include "net/asio_base.h"
#include "tools/type.h"
#include "ikcp.h"

#include <memory>
#include <map>

namespace ngl
{
	class kcp_session
	{
		kcp_session() = delete;
		kcp_session(const kcp_session&) = delete;
		kcp_session& operator=(const kcp_session&) = delete;

		std::map<i32_sessionid, std::shared_ptr<kcp_endpoint>>						m_dataofsession;		// Session id -> endpoint.
		std::map<i64_actorid, i32_sessionid>				m_actoridofsession;		// Actor id -> session id for both client and server ids.

		std::map<std::string, std::map<i16_port, std::shared_ptr<kcp_endpoint>>>	m_dataofendpoint;		// IP -> port -> endpoint.
		int32_t												m_sessionid = 0;		// Monotonic session id allocator.
		std::shared_mutex									m_mutex;				// Protects all lookup tables.
		asio_kcp*											m_asiokcp = nullptr;	// Back-reference used by new endpoints.

		// Internal helpers that assume the caller already holds m_mutex.
		std::shared_ptr<kcp_endpoint> erase_session_nolock(i32_sessionid asession);
		std::shared_ptr<kcp_endpoint> find_info(i32_sessionid asession);
		std::shared_ptr<kcp_endpoint> find_info(const basio::ip::udp::endpoint& aendpoint);
	public:
		enum
		{
			ekcp_update_intervalms = 10,		// kcp api update ( )
		};

		kcp_session(asio_kcp* asiokcp);

		// Create, reset, erase, find, and iterate KCP session entries.
		std::shared_ptr<kcp_endpoint> add_kcp_endpoint(
			int32_t aconv
			, const char* aip
			, i16_port aport
			, i64_actorid aactoridserver
			, i64_actorid aactoridclient
		);

		std::shared_ptr<kcp_endpoint> add(int32_t aconv, const basio::ip::udp::endpoint& aendpoint, i64_actorid aactoridserver, i64_actorid aactoridclient);

		std::shared_ptr<kcp_endpoint> reset_add(int32_t aconv, const basio::ip::udp::endpoint& aendpoint, i64_actorid aactoridserver, i64_actorid aactoridclient);

		void erasebysession(i32_sessionid asession);

		void erasebyaactorid(i64_actorid aactorid);

		std::shared_ptr<kcp_endpoint> find(i32_sessionid asession);

		std::shared_ptr<kcp_endpoint> findbyactorid(i64_actorid aactorid);

		std::shared_ptr<kcp_endpoint> find(const basio::ip::udp::endpoint& aendpoint);

		basio::ip::udp::endpoint* find_endpoint(i32_sessionid asession);

		void foreach(const std::function<void(std::shared_ptr<kcp_endpoint>&)>& acall);

		void foreachbyarea(i16_area aarea, const std::function<void(std::shared_ptr<kcp_endpoint>&)>& acall);
	};
}//namespace ngl
