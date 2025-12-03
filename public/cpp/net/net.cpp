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
#include "server_session.h"
#include "protocol.h"
#include "net_tcp.h"
#include "nguid.h"
#include "net.h"

namespace ngl
{
	std::array<net_protocol*, ENET_COUNT> nets::m_net;
	std::map<int16_t, ukcp*> nets::m_kcpnet;
	int16_t nets::m_kcpindex = 0;
	net_works nets::m_kcpworks;

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

	ukcp* nets::kcp(int16_t auport)
	{
		auto itor = m_kcpnet.find(auport);
		if (itor == m_kcpnet.end())
		{
			return nullptr;
		}
		return itor->second;
	}

	ukcp* nets::serkcp(pbnet::ENUM_KCP anum, int16_t atcount)
	{
		auto itor = m_kcpnet.find(kcp_port(m_kcpworks, atcount, anum));
		if (itor == m_kcpnet.end())
		{
			return nullptr;
		}
		return itor->second;
	}

	int16_t nets::kcp_port(const net_works& aitem, int16_t atcount, pbnet::ENUM_KCP aenum)
	{
		return aitem.m_port + aenum + atcount * 1000;
	}

	int16_t nets::kcp_ipport(int32_t atid, int16_t atcount, pbnet::ENUM_KCP aenum)
	{
		const tab_servers* tab = ttab_servers::instance().tab();
		if (tab == nullptr)
		{
			return -1;
		}
		for (const net_works& item : tab->m_net)
		{
			if (item.m_type == ENET_KCP)
			{
				return kcp_port(item, atcount, aenum);
			}
		}
		return -1;
	}

	int16_t nets::create_kcp()
	{
		if (m_kcpindex == 0)
		{
			m_kcpindex = nconfig::m_tcount*1000;
		}
		++m_kcpindex;
		m_kcpnet[m_kcpindex] = ukcp::create(m_kcpindex);
		return m_kcpindex;
	}

	int16_t nets::create_kcp(pbnet::ENUM_KCP aenum)
	{
		int16_t luport = kcp_port(m_kcpworks, nconfig::m_tcount, aenum);
		if (!m_kcpnet.contains(luport))
		{
			m_kcpnet[luport] = ukcp::create(luport);
			return luport;
		}
		return luport;
	}

	bool nets::init(i32_threadsize asocketthreadnum, bool aouternet, const std::set<pbnet::ENUM_KCP>& akcp)
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
		std::map<ENET_PROTOCOL, bool> lfirst;
		bool lfirstkcp = true;
		for (const net_works& item : tab->m_net)
		{
			if (lfirst.contains(item.m_type) && m_net[item.m_type] != nullptr)
			{
				tools::no_core_dump();
			}
			lfirst[item.m_type] = true;
			net_works lnwork;
			if (!ttab_servers::instance().get_nworks(item.m_type, nconfig::m_tcount, lnwork))
			{
				return true;
			}

			if (item.m_type == ENET_TCP || item.m_type == ENET_WS)
			{
				auto& lserver = m_net[item.m_type];
				if (item.m_type == ENET_TCP)
				{
					lserver = new net_tcp(ENET_TCP);
				}
				lserver->init(lnwork.m_port, asocketthreadnum, aouternet);
			}
			else if (item.m_type == ENET_KCP && nconfig::node_type() != ROBOT)
			{
				m_kcpworks = lnwork;
				for (pbnet::ENUM_KCP kcptype : akcp)
				{
					nets::create_kcp(kcptype);
				}
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