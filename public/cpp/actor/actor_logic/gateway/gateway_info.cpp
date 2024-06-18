#include "gateway_info.h"
#include "net.h"

namespace ngl
{
	void gateway_info::updata(gateway_socket& ainfo)
	{
		gateway_socket& linfo = m_info[ainfo.m_area][ainfo.m_dataid];
		if (linfo.m_socket != 0 && linfo.m_socket != ainfo.m_socket)
		{
			nets::net(linfo.m_socket)->close(linfo.m_socket);
			remove_socket(linfo.m_socket);
		}
		linfo = ainfo;
		if (ainfo.m_socket != 0)
			m_sockinfo[ainfo.m_socket] = &linfo;
	}

	bool gateway_info::updata_socket(i16_area aarea, i32_actordataid aactordataid, i32_socket asocket)
	{
		gateway_socket* ltemp = get(aarea, aactordataid);
		if (ltemp->m_socket != 0 && ltemp->m_socket != asocket)
		{
			remove_socket(ltemp->m_socket);
		}
		ltemp->m_socket = asocket;
		if (asocket != 0)
			m_sockinfo[asocket] = ltemp;
		return true;
	}

	std::map<i16_area, std::map<i32_actordataid, gateway_socket>>& gateway_info::info()
	{
		return m_info;
	}

	void gateway_info::remove_socket(i32_socket asocket)
	{
		if (sysconfig::robot_test())
		{
			for (std::pair<const i16_area, std::map<i32_actordataid, gateway_socket>>& item : m_info)
			{
				for (std::pair<const i32_actordataid, gateway_socket>& itemgateway : item.second)
				{
					if (itemgateway.second.m_socket == asocket)
					{
						itemgateway.second.m_socket = 0;
					}
				}
			}
			m_sockinfo.erase(asocket);
		}
		else
		{
			auto itor = m_sockinfo.find(asocket);
			if (itor == m_sockinfo.end())
				return;
			itor->second->m_socket = 0;
			m_sockinfo.erase(itor);
		}
	}

	void gateway_info::remove_actorid(i64_actorid aactorid)
	{
		i32_actordataid lactordataid = nguid::actordataid(aactorid);
		i32_actordataid larea = nguid::area(aactorid);
		auto itor_area = m_info.find(larea);
		if (itor_area == m_info.end())
			return;
		auto itor_actordataid = itor_area->second.find(lactordataid);
		if (itor_actordataid == itor_area->second.end())
			return;
		i32_socket lsocket = itor_actordataid->second.m_socket;
		if (lsocket != 0)
		{
			m_sockinfo.erase(lsocket);
		}
		itor_area->second.erase(itor_actordataid);
	}

	gateway_socket* gateway_info::get(i16_area aarea, i32_actordataid aroleid)
	{
		auto itor = m_info.find(aarea);
		if (itor == m_info.end())
			return nullptr;
		auto itor2 = itor->second.find(aroleid);
		if (itor2 == itor->second.end())
			return nullptr;
		return &itor2->second;
	}

	gateway_socket* gateway_info::get(i32_socket asocket)
	{
		auto itor = m_sockinfo.find(asocket);
		if (itor == m_sockinfo.end())
			return nullptr;
		return itor->second;
	}

	void gateway_info::get_subscribe_to(std::map<i64_actorid, i32_serverid>& m_subscribe_to)
	{
		for (const auto& [_area, _map] : m_info)
		{
			for (const auto& [_roledataid, _info] : _map)
			{
				m_subscribe_to.insert(std::make_pair(ngl::nguid::make(ACTOR_ROLE, _area, _roledataid), _info.m_gatewayid));
			}
		}
	}

	int64_t gateway_info::get_gatewayid(i64_actorid aid)
	{
		auto itor = m_info.find(ngl::nguid::area(aid));
		if (itor == m_info.end())
		{
			return -1;
		}
		auto itorinfo = itor->second.find(ngl::nguid::actordataid(aid));
		return itorinfo->second.m_gatewayid;
	}

	void gateway_info::foreach(const std::function<void(gateway_socket*)>& afun)
	{
		for (std::pair<const i16_area, std::map<i32_actordataid, gateway_socket>>& item : m_info)
		{
			for (std::pair<const i32_actordataid, gateway_socket>& itemgateway : item.second)
			{
				afun(&itemgateway.second);
			}
		}
	}
}//namespace ngl