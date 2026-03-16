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

#include "actor/actor_base/actor_base.h"
#include "net/udp/kcp/asio_kcp.h"
#include "net/udp/kcp/ukcp.h"

#include <unordered_map>

namespace ngl
{
	class nkcp
	{
		std::unordered_map<i16_port, std::shared_ptr<ukcp>> m_kcpnet;
		int16_t m_kcpindex;
	public:
		enum
		{
			enets_robot_tid = 100,
			enets_robot_tcount = 10,
			enets_kcp_tid = 1000,
			enets_kcp_tcount = 1000,
		};

		static nkcp& instance()
		{
			static nkcp ltemp;
			return ltemp;
		}

		i16_port kcp_port(int32_t atid, int16_t atcount, pbnet::ENUM_KCP aenum);

		// # Getinstance
		std::shared_ptr<ukcp> kcp(i16_port auport);
		std::shared_ptr<ukcp> serkcp(pbnet::ENUM_KCP aenum, int16_t atcount);

		// # Robot create port
		i16_port create_kcp();

		// # Server createspecifiedport
		i16_port create_kcp(pbnet::ENUM_KCP aenum);
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool actor_base::kcp_send(i64_actorid aactorid, T& adata, i16_port auport/* = 0*/)
	{
		std::set<i64_actorid> lactorids = { aactorid };
		return kcp_send(lactorids, adata, auport);
	}

	template <typename T>
	bool actor_base::kcp_send(const std::set<i64_actorid>& aactorids, T& adata, i16_port auport/* = 0*/)
	{
		auto lpukcp = nkcp::instance().kcp(auport);
		if (lpukcp == nullptr)
		{
			return false;
		}
		return lpukcp->send(aactorids, adata);
	}
}//namespace ngl