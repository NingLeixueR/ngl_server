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

#include "net/udp/kcp/nkcp.h"
#include "tools/tools.h"

namespace ngl
{
	i16_port nkcp::kcp_port(int32_t atid, int16_t atcount, pbnet::ENUM_KCP aenum)
	{
		return (atid * enets_kcp_tid) + aenum + (atcount * enets_kcp_tcount);
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
			m_kcpindex = nconfig.tid() * enets_robot_tid + nconfig.tcount() * enets_robot_tcount;
		}
		++m_kcpindex;
		m_kcpnet[m_kcpindex] = ukcp::create(m_kcpindex);
		return m_kcpindex;
	}

	i16_port nkcp::create_kcp(pbnet::ENUM_KCP aenum)
	{
		i16_port luport = kcp_port(nconfig.tid(), nconfig.tcount(), aenum);
		if (!m_kcpnet.contains(luport))
		{
			m_kcpnet[luport] = ukcp::create(luport);
			return luport;
		}
		return luport;
	}
}//namespace ngl