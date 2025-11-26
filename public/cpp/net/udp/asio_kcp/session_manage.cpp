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
		m_sessionid(((int32_t)ENET_KCP << 24)),
		m_asiokcp(asiokcp)
	{}

	ptr_se session_manage::add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactorid)
	{
		std::string lip = aendpoint.address().to_string();
		i16_port lport = aendpoint.port();
		monopoly_shared_lock(m_mutex);
		auto itor = m_dataofendpoint.find(lip);
		if (itor != m_dataofendpoint.end())
		{
			auto itorport = itor->second.find(lport);
			if (itorport != itor->second.end())
			{
				return itorport->second;
			}
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
		ltemp->m_actorid = nguid::make(ACTOR_ROLE, nguid::area(aactorid), nguid::actordataid(aactorid));
		ltemp->create(aconv, m_sessionid, (void*)ltemp.get());

		//设置kcp对象的回调函数
		ltemp->setoutput(udp_output);
		ltemp->nodelay(1, 10, 2, 1);
		ltemp->wndsize(128, 128);
		ltemp->setmtu(512);
		m_dataofendpoint[lip][lport] = ltemp;

		int64_t lcreatems = time_wheel::getms();
		wheel_parm lparm
		{
			.m_ms = 10,
			.m_intervalms = [](int64_t) {return 10; } ,
			.m_count = 0x7fffffff,
			.m_fun = [ltemp,lcreatems](const wheel_node*)
			{
				ltemp->update((IUINT32)(time_wheel::getms() - lcreatems));
			}
		};
		m_kcptimer.addtimer(lparm);
		return ltemp;
	}

	ptr_se session_manage::reset_add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactorid)
	{
		erase(aendpoint);
		return add(aconv, aendpoint, aactorid);
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
		auto itor = m_dataofsession.find(asession);
		if (itor == m_dataofsession.end())
			return nullptr;
		return itor->second;
	}

	ptr_se session_manage::_find(const asio_udp_endpoint& aendpoint)
	{
		std::string lip = aendpoint.address().to_string();
		i16_port lport = aendpoint.port();
		auto itor = m_dataofendpoint.find(lip);
		if (itor != m_dataofendpoint.end())
		{
			auto itorport = itor->second.find(lport);
			if (itorport != itor->second.end())
			{
				return itorport->second;
			}
		}
		return nullptr;
	}

	ptr_se session_manage::find(i32_sessionid asession)
	{
		monopoly_shared_lock(m_mutex);
		auto itor = m_dataofsession.find(asession);
		if (itor == m_dataofsession.end())
		{
			return nullptr;
		}
		return itor->second;
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



}//namespace ngl