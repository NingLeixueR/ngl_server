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
#include "kcp_session.h"
#include "nprotocol.h"
#include "asio_kcp.h"
#include "nguid.h"
#include "nlog.h"

namespace ngl
{
	int udp_output(const char* buf, int len, ikcpcb* kcp, void* user)
	{
		const auto lpstruct = (kcp_endpoint*)user;
		lpstruct->m_asiokcp->sendbuff(lpstruct->m_endpoint, buf, len);
		return len;
	}

	kcp_session::kcp_session(asio_kcp* asiokcp) :
		m_sessionid(0),
		m_asiokcp(asiokcp)
	{}

	std::shared_ptr<kcp_endpoint> kcp_session::add_kcp_endpoint(
		int32_t aconv
		, const char* aip
		, i16_port aport
		, i64_actorid aactoridserver
		, i64_actorid aactoridclient
	)
	{
		auto ltemp = std::make_shared<kcp_endpoint>();
		
		ltemp->m_ip = aip;
		ltemp->m_port = aport;
		ltemp->m_asiokcp = m_asiokcp;
		ltemp->m_actoridclient = aactoridclient;
		ltemp->m_actoridserver = aactoridserver;
		ltemp->create(aconv, m_sessionid, (void*)ltemp.get());
		
		//设置kcp对象的回调函数
		ltemp->setoutput(udp_output);
		ltemp->nodelay(1, 10, 2, 1);
		ltemp->wndsize(128, 128);
		ltemp->setmtu(512);

		return ltemp;
	}

	ptr_se kcp_session::add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactoridserver, i64_actorid aactoridclient)
	{
		if (aconv <= 0)
		{
			return nullptr;
		}
		std::string lip = aendpoint.address().to_string();
		i16_port lport = aendpoint.port();
		int64_t lcreatems = time_wheel::getms();
		auto ltemp = add_kcp_endpoint(aconv, lip.c_str(), lport, aactoridserver, aactoridclient);
		ltemp->m_endpoint = aendpoint;

		{
			monopoly_shared_lock(m_mutex);
			erasebyaactorid(aactoridclient);
			i32_sessionid lsessionid = ++m_sessionid;
			m_dataofsession[lsessionid] = ltemp;
			ltemp->m_session = lsessionid;
			m_actoridofsession[aactoridclient] = lsessionid;
			m_actoridofsession[aactoridserver] = lsessionid;
			m_dataofendpoint[lip][lport] = ltemp;
			wheel_parm lparm
			{
				.m_ms = ekcp_update_intervalms,
				.m_intervalms = [](int64_t) {return ekcp_update_intervalms; } ,
				.m_count = 0x7fffffff,
				.m_fun = [this, lsessionid, lcreatems](const wheel_node*)
				{
					ptr_se* lpse = nullptr;
					{
						monopoly_shared_lock(m_mutex);
						lpse = tools::findmap(m_dataofsession, lsessionid);
					}
					if (lpse != nullptr)
					{
						(*lpse)->update((IUINT32)(time_wheel::getms() - lcreatems));
					}
				}
			};
			ltemp->m_timerid = m_kcptimer.addtimer(lparm);
		}
		
		return ltemp;
	}

	ptr_se kcp_session::reset_add(int32_t aconv, const asio_udp_endpoint& aendpoint, i64_actorid aactoridserver, i64_actorid aactoridclient)
	{
		return add(aconv, aendpoint, aactoridserver, aactoridclient);
	}

	void kcp_session::erasebysession(i32_sessionid asession)
	{
		monopoly_shared_lock(m_mutex);
		ptr_se lpse = nullptr;
		if (tools::erasemap(m_dataofsession, asession, lpse))
		{
			if (!tools::erasemap(m_dataofendpoint[lpse->m_ip], lpse->m_port, lpse))
			{
				log_error()->print("kcp_session erasebysession fail ip:{} port:{}", lpse->m_ip, lpse->m_port);
			}
			i32_sessionid lsession = 0;
			if (!tools::erasemap(m_actoridofsession, lpse->m_actoridclient, lsession))
			{
				log_error()->print("kcp_session erasebysession fail session:{}", asession);
			}
			if (lsession != asession)
			{
				log_error()->print("kcp_session erasebysession fail session1:{} session2:{}", lsession, asession);
			}
		}
	}

	void kcp_session::erasebyaactorid(i64_actorid aactorid)
	{
		i32_sessionid lsessionid = 0;
		if (!tools::erasemap(m_actoridofsession, aactorid, lsessionid))
		{
			ptr_se lpse = nullptr;
			if (tools::erasemap(m_dataofsession, lsessionid, lpse))
			{
				tools::erasemap(m_dataofendpoint[lpse->m_ip], lpse->m_port, lpse);
			}
		}
	}

	ptr_se kcp_session::find_info(i32_sessionid asession)
	{
		auto lpse = tools::findmap(m_dataofsession, asession);
		if (lpse == nullptr)
		{
			return nullptr;
		}
		return *lpse;
	}

	ptr_se kcp_session::find_info(const asio_udp_endpoint& aendpoint)
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

	ptr_se kcp_session::find(i32_sessionid asession)
	{
		monopoly_shared_lock(m_mutex);
		return find_info(asession);
	}

	ptr_se kcp_session::findbyactorid(i64_actorid aactorid)
	{
		monopoly_shared_lock(m_mutex);
		auto lpse = tools::findmap(m_actoridofsession, aactorid);
		if (lpse == nullptr)
		{
			return nullptr;
		}
		return find_info(*lpse);
	}

	ptr_se kcp_session::find(const asio_udp_endpoint& aendpoint)
	{
		monopoly_shared_lock(m_mutex);
		return find_info(aendpoint);
	}

	asio_udp_endpoint* kcp_session::find_endpoint(i32_sessionid asession)
	{
		monopoly_shared_lock(m_mutex);
		ptr_se lpstruct = find_info(asession);
		if (lpstruct == nullptr)
		{
			return nullptr;
		}
		return &lpstruct->m_endpoint;
	}

	void kcp_session::foreach(const std::function<void(ptr_se&)>& acall)
	{
		for (auto& [_session, _se] : m_dataofsession)
		{
			acall(_se);
		}
	}

	void kcp_session::foreachbyarea(i16_area aarea, const std::function<void(ptr_se&)>& acall)
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