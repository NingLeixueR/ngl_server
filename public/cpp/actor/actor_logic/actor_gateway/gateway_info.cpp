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
// File overview: Implements logic for actor gateway.


#include "actor/actor_logic/actor_gateway/gateway_info.h"
#include "net/nnet.h"

namespace ngl
{
	void gateway_info::updata(const gateway_socket& ainfo)
	{
		auto lmap_it = m_info.try_emplace(ainfo.m_area).first;
		auto& lmap = lmap_it->second;
		auto [litor, _] = lmap.try_emplace(ainfo.m_dataid);
		gateway_socket* lpgsock = &litor->second;
		if (lpgsock->m_socket > 0 && lpgsock->m_socket != ainfo.m_socket)
		{
			nnet::instance().close(lpgsock->m_socket);
			remove_socket(lpgsock->m_socket);
		}

		*lpgsock = ainfo;
		if (ainfo.m_socket != 0)
		{
			m_sockinfo.insert_or_assign(ainfo.m_socket, lpgsock);
		}
	}

	bool gateway_info::updata_socket(i16_area aarea, i32_actordataid aactordataid, i32_socket asocket)
	{
		gateway_socket* ltemp = get(aarea, aactordataid);
		if (ltemp == nullptr)
		{
			return false;
		}
		if (ltemp->m_socket != 0 && ltemp->m_socket != asocket)
		{
			remove_socket(ltemp->m_socket);
		}
		ltemp->m_socket = asocket;
		if (asocket != 0)
		{
			m_sockinfo.insert_or_assign(asocket, ltemp);
		}
		return true;
	}

	std::map<i16_area, std::map<i32_actordataid, gateway_socket>>& gateway_info::info()
	{
		return m_info;
	}

	void gateway_info::remove_socket(i32_socket asocket)
	{
		gateway_socket* lpgateway;
		if (tools::erasemap(m_sockinfo, asocket, lpgateway))
		{
			lpgateway->m_socket = 0;
		}
	}

	void gateway_info::remove_actorid(i64_actorid aactorid)
	{
		i32_actordataid lactordataid = nguid::actordataid(aactorid);
		i16_area larea = nguid::area(aactorid);

		auto lpmap = tools::findmap(m_info, larea);
		if (lpmap == nullptr)
		{
			return;
		}
		gateway_socket* lgateway = tools::findmap(*lpmap, lactordataid);
		if (lgateway == nullptr)
		{
			return;
		}
		if (lgateway->m_socket != 0)
		{
			m_sockinfo.erase(lgateway->m_socket);
		}
		lpmap->erase(lactordataid);
	}

	gateway_socket* gateway_info::get(i16_area aarea, i32_actordataid aroleid)
	{
		auto lpmap = tools::findmap(m_info, aarea);
		if (lpmap == nullptr)
		{
			return nullptr;
		}
		return tools::findmap(*lpmap, aroleid);
	}

	gateway_socket* gateway_info::get(i32_socket asocket)
	{
		auto lpsocket = tools::findmap(m_sockinfo, asocket);
		if (lpsocket == nullptr)
		{
			return nullptr;
		}
		return *lpsocket;
	}

	int64_t gateway_info::gatewayid(i64_actorid aid)
	{
		auto lpmap = tools::findmap(m_info, ngl::nguid::area(aid));
		if (lpmap == nullptr)
		{
			return -1;
		}
		auto lpinfor = tools::findmap(*lpmap, ngl::nguid::actordataid(aid));
		if (lpinfor == nullptr)
		{
			return -1;
		}
		return lpinfor->m_gatewayid;
	}

	void gateway_info::foreach(const std::function<void(gateway_socket*)>& afun)
	{
		for (auto& lpair : m_info)
		{
			for (auto& litem : lpair.second)
			{
				afun(&litem.second);
			}
		}
	}

	void gateway_info::updata(const np_actor_gatewayinfo_updata& adata)
	{
		for (const i32_socket lsock : adata.m_delsocket)
		{
			remove_socket(lsock);
		}
		for (const i64_actorid lactorid : adata.m_delactorid)
		{
			remove_actorid(lactorid);
		}
		for (const gateway_socket& lgate : adata.m_add)
		{
			updata(lgate);
		}
	}
}//namespace ngl
