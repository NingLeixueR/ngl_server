#include "tools.h"
#include "nkcp.h"


namespace ngl
{
	i16_port nkcp::kcp_port(int32_t atid, int16_t atcount, pbnet::ENUM_KCP aenum)
	{
		return (atid * enets_kcp_tid) + aenum + (atcount * enets_kcp_tcount);
	}

	ukcp* nkcp::kcp(i16_port auport)
	{
		ukcp** lpukcp = tools::findmap(m_kcpnet, auport);
		if (lpukcp == nullptr)
		{
			return nullptr;
		}
		return *lpukcp;
	}

	ukcp* nkcp::serkcp(pbnet::ENUM_KCP anum, int16_t atcount)
	{
		ukcp** lpukcp = tools::findmap(m_kcpnet, kcp_port(nconfig.tid(), atcount, anum));
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