#include "server_session.h"
#include "protocol.h"
#include "net_tcp.h"
#include "nguid.h"
#include "net.h"

namespace ngl
{
	std::array<net_protocol*, ENET_COUNT> nets::m_net;
	std::map<int16_t, ukcp*> nets::m_kcpnet;
	int16_t nets::m_kcpindex = 1000;

	net_protocol* nets::net_first()
	{
		for (int i = 0; i < ENET_COUNT; ++i)
		{
			if (m_net[i] != nullptr)
			{
				return m_net[i];
			}
		}
		return nullptr;
	}

	ENET_PROTOCOL nets::session2type(i32_session asession)
	{
		return (ENET_PROTOCOL)(asession >> 24);
	}

	net_protocol* nets::net(i32_session asession)
	{
		ENET_PROTOCOL ltype = session2type(asession);
		return nettype(ltype);
	}

	net_protocol* nets::nettype(ENET_PROTOCOL atype)
	{
		if (ttab_servers::instance().isefficient(atype))
		{
			if (m_net.size() > atype)
			{
				return m_net[atype];
			}
		}
		return nullptr;
	}

	bool nets::check_serverkcp()
	{
		const tab_servers* tab = ttab_servers::instance().tab();
		if (tab == nullptr)
		{
			return false;
		}
		int32_t lnum = 0;
		for (const net_works& item : tab->m_net)
		{
			if (item.m_type == ENET_KCP && item.m_port != -1)
			{
				++lnum;
				if (lnum > 1)
				{
					return false;
				}
			}
		}
		return true;
	}

	ukcp* nets::kcp(int16_t anum /*= isystemindex*/)
	{
		auto itor = m_kcpnet.find(anum);
		if (itor == m_kcpnet.end())
		{
			return nullptr;
		}
		return itor->second;
	}

	int16_t nets::create_kcp()
	{
		++m_kcpindex;
		m_kcpnet[m_kcpindex] = ukcp::create(m_kcpindex);
		return m_kcpindex;
	}

	bool nets::init(i32_threadsize asocketthreadnum, bool aouternet)
	{
		if (check_serverkcp() == false)
		{
			return false;
		}

		const tab_servers* tab = ttab_servers::instance().tab();
		if (tab == nullptr)
		{
			return false;
		}
		
		for (const net_works& item : tab->m_net)
		{
			uint16_t lport = item.m_port + (nconfig::m_tcount-1);
			if (item.m_type == ENET_TCP || item.m_type == ENET_WS)
			{
				if (m_net[item.m_type] != nullptr)
				{
					continue;
				}
				auto& lserver = m_net[item.m_type];
				if (item.m_type == ENET_TCP)
				{
					lserver = new net_tcp((int8_t)ENET_TCP);
				}
				lserver->init(lport, asocketthreadnum, aouternet);
			}
			else if (item.m_type == ENET_KCP&& lport != -1)
			{
				m_kcpnet[isystemindex] = ukcp::create(lport);
			}
		}
		return true;
	}

	bool nets::sendpack(i32_sessionid asession, std::shared_ptr<pack>& apack)
	{
		net_protocol* lpprotocol = net(asession);
		if (lpprotocol == nullptr)
		{
			return false;
		}
		return lpprotocol->net_send(asession, apack);
	}

	bool nets::sendpack(i32_sessionid asession, std::shared_ptr<void>& apack)
	{
		net_protocol* lpprotocol = net(asession);
		if (lpprotocol == nullptr)
		{
			return false;
		}
		return lpprotocol->net_send(asession, apack);
	}

	bool nets::sendmsg(i32_sessionid asession, const std::string& amsg)
	{
		auto lcount = (int)amsg.size();
		auto lpack = std::make_shared<pack>();
		lpack->malloc(lcount+1);
		memcpy(lpack->m_buff, amsg.c_str(), lcount);
		lpack->m_buff[lcount] = '\0';
		lpack->m_len = lcount+1;
		lpack->m_pos = lcount+1;
		return nets::sendpack(asession, lpack);
	}

	bool nets::ipport(i32_serverid aserverid, std::tuple<ENET_PROTOCOL, str_ip, i16_port>& apair)
	{
		net_works lnets;
		if (!ttab_servers::instance().connect(aserverid, lnets))
		{
			return false;
		}
		if (nconfig::m_nodetype == ROBOT)
		{
			apair = std::make_tuple(lnets.m_type, lnets.m_ip, lnets.m_port);
		}
		else
		{
			apair = std::make_tuple(lnets.m_type, lnets.m_nip, lnets.m_port);
		}
		return true;
	}

	bool nets::connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession != -1)
		{
			afun(lsession);
			return true;
		}
		std::tuple<ENET_PROTOCOL, str_ip, i16_port> lpair;
		if (!ipport(aserverid, lpair))
		{
			return false;
		}
		
		net_protocol* lserver = m_net[std::get<0>(lpair)];
		if (lserver == nullptr)
		{
			return false;
		}
		log_info()->print("Connect Server {}@{}:{}", aserverid, std::get<1>(lpair), std::get<2>(lpair));

		return lserver->connect(std::get<1>(lpair), std::get<2>(lpair), [aserverid, afun](i32_session asession)
			{
				server_session::add(aserverid, asession);
				if (afun != nullptr)
				{
					afun(asession);
				}
			}, await, areconnection);
	}

	bool handle_pram::netsendpack(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return nets::net(lsession)->sendpack(lsession, apack);
	}

	bool handle_pram::netsendpack(i32_serverid aserverid, std::shared_ptr<void>& apack)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return nets::net(lsession)->sendpack(lsession, apack);
	}
}//namespace ngl