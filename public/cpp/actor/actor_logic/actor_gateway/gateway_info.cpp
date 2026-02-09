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
#include "gateway_info.h"
#include "ntcp.h"

namespace ngl
{
	void gateway_info::updata(const gateway_socket& ainfo)
	{
		gateway_socket* lpgsocket = tools::findmap(m_info[ainfo.m_area], ainfo.m_dataid);
		if (lpgsocket != nullptr)
		{
			if (lpgsocket->m_socket > 0 && lpgsocket->m_socket != ainfo.m_socket)
			{
				ntcp::instance().close(lpgsocket->m_socket);
				remove_socket(lpgsocket->m_socket);
			}
		}
		else
		{
			lpgsocket = &m_info[ainfo.m_area][ainfo.m_dataid];
		}

		*lpgsocket = ainfo;
		if (ainfo.m_socket != 0)
		{
			m_sockinfo[ainfo.m_socket] = lpgsocket;
		}
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
		{
			m_sockinfo[asocket] = ltemp;
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
		auto lpsocket = tools::findmap(*lpmap, aroleid);
		if(lpsocket == nullptr)
		{
			return nullptr;
		}
		return lpsocket;
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
		for (auto& [_area, _map] : m_info)
		{
			for (auto& [_dataid, _socket] : _map)
			{
				afun(&_socket);
			}
		}
	}

	void gateway_info::updata(const np_actor_gatewayinfo_updata& adata)
	{
		for (i32_socket asocket : adata.m_delsocket)
		{
			remove_socket(asocket);
		}
		for (i64_actorid aactorid : adata.m_delactorid)
		{
			remove_actorid(aactorid);
		}
		for (const gateway_socket& agetway : adata.m_add)
		{
			updata(agetway);
		}
	}
}//namespace ngl