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
#include "session_manage.h"
#include "asio_kcp.h"
#include "nguid.h"

namespace ngl
{
	int udp_output(const char* buf, int len, ikcpcb* kcp, void* user)
	{
		const auto lpstruct = (session_endpoint*)user;
		lpstruct->m_asiokcp->sendbuff(lpstruct->m_endpoint, buf, len);
		return len;
	}

	session_manage::session_manage(asio_kcp* asiokcp) :
		m_sessionid(0),
		m_asiokcp(asiokcp)
	{}

	ptr_se session_manage::add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactoridclient, i64_actorid aactoridserver)
	{
		std::string lip = aendpoint.address().to_string();
		i16_port lport = aendpoint.port();
		monopoly_shared_lock(m_mutex);
		auto lpmap = tools::findmap(m_dataofendpoint, lip);
		if (lpmap != nullptr)
		{
			auto lpse =  tools::findmap(*lpmap, lport);
			if (lpmap == nullptr)
			{
				return nullptr;
			}
			return *lpse;
		}
		if (aconv <= 0)
		{
			return nullptr;
		}
		auto ltemp = std::make_shared<session_endpoint>();
		m_dataofsession[++m_sessionid] = ltemp;
		ltemp->m_session = m_sessionid;
		ltemp->m_endpoint = aendpoint;
		ltemp->m_ip = lip;
		ltemp->m_port = lport;
		ltemp->m_asiokcp = m_asiokcp;
		ltemp->m_actoridclient = aactoridclient;
		ltemp->m_actoridserver = aactoridserver;
		ltemp->create(aconv, m_sessionid, (void*)ltemp.get());
		m_actoridofsession[aactoridclient] = m_sessionid;
		m_actoridofsession[aactoridserver] = m_sessionid;

		//设置kcp对象的回调函数
		ltemp->setoutput(udp_output);
		ltemp->nodelay(1, 10, 2, 1);
		ltemp->wndsize(128, 128);
		ltemp->setmtu(512);
		m_dataofendpoint[lip][lport] = ltemp;

		int64_t lcreatems = time_wheel::getms();
		wheel_parm lparm
		{
			.m_ms = ekcp_update_intervalms,
			.m_intervalms = [](int64_t) {return ekcp_update_intervalms; } ,
			.m_count = 0x7fffffff,
			.m_fun = [ltemp,lcreatems](const wheel_node*)
			{
				ltemp->update((IUINT32)(time_wheel::getms() - lcreatems));
			}
		};
		m_kcptimer.addtimer(lparm);
		return ltemp;
	}

	ptr_se session_manage::reset_add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactoridlocal, i64_actorid aactoridremote)
	{
		erase(aendpoint);
		return add(aconv, aendpoint, aactoridlocal, aactoridremote);
	}

	void session_manage::erase(const asio_udp_endpoint& aendpoint)
	{
		std::string lip = aendpoint.address().to_string();
		i16_port lport = aendpoint.port();
		monopoly_shared_lock(m_mutex);
		ptr_se lpstruct = _find(aendpoint);
		if (lpstruct == nullptr)
		{
			return;
		}
		auto& lmap = m_dataofendpoint[lpstruct->m_ip];
		lmap.erase(lpstruct->m_port);
		if (lmap.empty())
		{
			m_dataofendpoint.erase(lpstruct->m_ip);
		}
		m_dataofsession.erase(lpstruct->m_session);
	}

	void session_manage::erase(i32_sessionid asession)
	{
		monopoly_shared_lock(m_mutex);
		ptr_se lpstruct = _find(asession);
		if (lpstruct == nullptr)
		{
			return;
		}
		auto& lmap = m_dataofendpoint[lpstruct->m_ip];
		lmap.erase(lpstruct->m_port);
		if (lmap.empty())
		{
			m_dataofendpoint.erase(lpstruct->m_ip);
		}
		m_dataofsession.erase(asession);
	}

	ptr_se session_manage::_find(i32_sessionid asession)
	{
		auto lpse = tools::findmap(m_dataofsession, asession);
		if (lpse == nullptr)
		{
			return nullptr;
		}
		return *lpse;
	}

	ptr_se session_manage::_find(const asio_udp_endpoint& aendpoint)
	{
		std::string lip = aendpoint.address().to_string();
		i16_port lport = aendpoint.port();
	 	auto lpmap = tools::findmap(m_dataofendpoint, lip);
		if (lpmap == nullptr)
		{
			return nullptr;
		}
		auto lpse = tools::findmap(*lpmap, lport);
		if (lpse == nullptr)
		{
			return nullptr;
		}
		return *lpse;
	}

	ptr_se session_manage::find(i32_sessionid asession)
	{
		monopoly_shared_lock(m_mutex);
		return _find(asession);
	}

	ptr_se session_manage::findbyactorid(i64_actorid aactorid)
	{
		monopoly_shared_lock(m_mutex);
		auto lpse = tools::findmap(m_actoridofsession, aactorid);
		if (lpse == nullptr)
		{
			return nullptr;
		}
		return _find(*lpse);
	}

	ptr_se session_manage::find(const asio_udp_endpoint& aendpoint)
	{
		monopoly_shared_lock(m_mutex);
		return _find(aendpoint);
	}

	asio_udp_endpoint* session_manage::find_endpoint(i32_sessionid asession)
	{
		monopoly_shared_lock(m_mutex);
		ptr_se lpstruct = _find(asession);
		if (lpstruct == nullptr)
		{
			return nullptr;
		}
		return &lpstruct->m_endpoint;
	}

	void session_manage::foreach(const std::function<void(ptr_se&)>& acall)
	{
		for (auto& [_session, _se] : m_dataofsession)
		{
			acall(_se);
		}
	}

	void session_manage::foreachbyarea(i16_area aarea, const std::function<void(ptr_se&)>& acall)
	{
		for (std::pair<const i64_actorid, i32_sessionid>& lpair : m_actoridofsession)
		{
			if (nguid::area(lpair.first) == aarea)
			{
				ptr_se lptr = find(lpair.first);
				if (lptr != nullptr)
				{
					acall(lptr);
				}
			}
		}
	}
}//namespace ngl