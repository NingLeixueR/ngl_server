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
// File overview: Declares interfaces for kcp.

#pragma once

#include "net/udp/kcp/asio_kcp.h"
#include "net/net_pack.h"
#include "tools/type.h"

namespace ngl
{
	class ukcp
	{
		asio_kcp	m_kcp;  // Transport implementation bound to one local UDP port.
		bpool		m_pool; // Pack allocation pool for outbound traffic.

	public:
		// High-level KCP wrapper used by actor code: serialize payloads, resolve actors, and
		// delegate actual transport to asio_kcp.
		static const int32_t	m_conv = 1;
		static std::string		m_localuip;

		ukcp(i16_port aprot);

		static std::shared_ptr<ukcp> create(i16_port aprot);

#pragma region kcp_send
		template <typename T>
		bool send(const std::set<i64_actorid>& aactors, T& adata)
		{
			auto lpack = net_pack<T>::npack(&m_pool, adata, 0, 0);
			if (lpack == nullptr)
			{
				return false;
			}
			return send_server(aactors, lpack);
		}

		template <typename T>
		bool send(i64_actorid aactoridclient, T& adata)
		{
			std::set<i64_actorid> lactors = { aactoridclient };
			return send(lactors, adata);
		}

		bool send_server(std::shared_ptr<pack>& apack);

		bool send_server(const std::set<i64_actorid>& aactors, const std::shared_ptr<pack>& apack);

		bool send_server(i64_actorid aactor, const std::shared_ptr<pack>& apack);

		bool sendpackbyarea(i16_area aarea, const std::shared_ptr<pack>& apack);

		template <typename T>
		bool send(const asio_udp_endpoint& aendpoint, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			return m_kcp.send_server(aendpoint, lpack);
		}
#pragma endregion 

#pragma region udp_send
		template <typename T>
		bool sendu(const asio_udp_endpoint& aendpoint, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			return m_kcp.sendu(aendpoint, lpack->m_buff, lpack->m_len);
		}

		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		// Send a raw UDP request and block the caller until the matching reply arrives.
		bool sendu_waitrecv(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun);
#pragma endregion 

		// Connect and query actor/session relationships.
		void connect(
			std::string& akcpsess
			, i64_actorid aserver
			, i64_actorid aclient
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		);
		void connect(
			std::string& akcpsess
			, i64_actorid aserver
			, i64_actorid aclient
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		);

		i64_actorid find_server(i32_session asession);
		i64_actorid find_client(i32_session asession);
		bool find_actorid(i32_session asession, i64_actorid& aserver, i64_actorid& aclient);

		// Derive and verify the logical handshake token shared by client and server.
		static bool session_create(i64_actorid aclient, i64_actorid aserver, std::string& asession);

		static bool session_check(i64_actorid aclient, i64_actorid aserver, const std::string& asession);

		// Rebuild a session table entry for an already known remote endpoint.
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient);
		void reset_add(const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient);
	};
}// namespace ngl
