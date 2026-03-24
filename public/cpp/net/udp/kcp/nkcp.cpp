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
// File overview: Implements logic for kcp.


#include "net/udp/kcp/nkcp.h"
#include "tools/tools.h"

namespace ngl
{
	i16_port nkcp::kcp_port(int32_t atid, int16_t atcount, pbnet::ENUM_KCP aenum)
	{
		return static_cast<i16_port>((atid * enets_kcp_tid) + static_cast<int32_t>(aenum) + (atcount * enets_kcp_tcount));
	}

	std::shared_ptr<ukcp> nkcp::kcp(i16_port auport)
	{
		auto lpukcp = tools::findmap(m_kcpnet, auport);
		if (lpukcp == nullptr)
		{
			return nullptr;
		}
		return *lpukcp;
	}

	std::shared_ptr<ukcp> nkcp::serkcp(pbnet::ENUM_KCP anum, int16_t atcount)
	{
		auto lpukcp = tools::findmap(m_kcpnet, kcp_port(nconfig.tid(), atcount, anum));
		if (lpukcp == nullptr)
		{
			return nullptr;
		}
		return *lpukcp;
	}

	i16_port nkcp::create_kcp()
	{
		if (m_kcpindex == 0)
		{
			// Keep robot-side ephemeral ports in a dedicated numeric range.
			m_kcpindex = nconfig.tid() * enets_robot_tid + nconfig.tcount() * enets_robot_tcount;
		}
		++m_kcpindex;
		m_kcpnet.insert_or_assign(m_kcpindex, ukcp::create(m_kcpindex));
		return m_kcpindex;
	}

	i16_port nkcp::create_kcp(pbnet::ENUM_KCP aenum)
	{
		i16_port luport = kcp_port(nconfig.tid(), nconfig.tcount(), aenum);
		if (m_kcpnet.find(luport) == m_kcpnet.end())
		{
			m_kcpnet.emplace(luport, ukcp::create(luport));
		}
		return luport;
	}
}//namespace ngl
