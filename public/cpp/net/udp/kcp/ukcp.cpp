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

#include "actor/protocol/nprotocol.h"
#include "tools/tab/xml/sysconfig.h"
#include "actor/actor_base/nguid.h"
#include "net/udp/kcp/ukcp.h"
#include "tools/log/nlog.h"

namespace ngl
{
	std::string ukcp::m_localuip = "";

	ukcp::ukcp(i16_port aprot) :
		m_kcp(aprot)
	{}

	std::shared_ptr<ukcp> ukcp::create(i16_port aprot)
	{
		return std::make_shared<ukcp>(aprot);
	}

	bool ukcp::send_server(std::shared_ptr<pack>& apack)
	{
		return m_kcp.send_server(apack);
	}

	bool ukcp::send_server(const std::set<i64_actorid>& aactors, const std::shared_ptr<pack>& apack)
	{
		for (i64_actorid lactorcliend : aactors)
		{
			i32_session lsession = m_kcp.find_session(lactorcliend);
			if (lsession == -1)
			{
				continue;
			}
			i64_actorid lserver = 0;
			i64_actorid lclient = 0;
			if (!m_kcp.find_actorid(lsession, lserver, lclient))
			{
				continue;
			}
			if (nconfig.nodetype() != ROBOT)
			{
				pack_head::head_set_actor((int32_t*)apack->m_buff, lclient, lserver);
			}
			else
			{
				pack_head::head_set_actor((int32_t*)apack->m_buff, lserver, lclient);
			}
			m_kcp.send_server(lsession, apack);
		}
		return true;
	}

	bool ukcp::send_server(i64_actorid aactor, const std::shared_ptr<pack>& apack)
	{
		std::set<i64_actorid> lactors = { aactor };
		return send_server(lactors, apack);
	}

	bool ukcp::sendpackbyarea(i16_area aarea, const std::shared_ptr<pack>& apack)
	{
		return m_kcp.sendpackbyarea(aarea, apack);
	}

	bool ukcp::sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len)
	{
		return m_kcp.sendu(aendpoint, buf, len);
	}

	bool ukcp::sendu_waitrecv(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun)
	{
		std::unique_ptr<ngl::sem> lsem = std::make_unique<ngl::sem>();
		m_kcp.sendu_waitrecv(aendpoint, buf, len, [afun, &lsem](char* buff, int len)
			{
				afun(buff, len);
				lsem->post();
			}
		);
		lsem->wait();
		return true;
	}

	void ukcp::connect(std::string& akcpsess
		, i64_actorid aserver
		, i64_actorid aclient
		, const std::string& aip
		, i16_port aport
		, const std::function<void(i32_session)>& afun
	)
	{
		m_kcp.connect(m_conv, akcpsess, aserver, aclient, aip, aport, afun);
	}

	void ukcp::connect(std::string& akcpsess
		, i64_actorid aserver
		, i64_actorid aclient
		, const asio_udp_endpoint& aendpoint
		, const std::function<void(i32_session)>& afun
	)
	{
		m_kcp.connect(m_conv, akcpsess, aserver, aclient, aendpoint, afun);
	}

	i64_actorid ukcp::find_server(i32_session asession)
	{
		return m_kcp.find_server(asession);
	}

	i64_actorid ukcp::find_client(i32_session asession)
	{
		return m_kcp.find_client(asession);
	}

	bool ukcp::find_actorid(i32_session asession, i64_actorid& aserver, i64_actorid& aclient)
	{
		return m_kcp.find_actorid(asession, aserver, aclient);
	}

	// 生成kcp-session以验证连接
	bool ukcp::session_create(i64_actorid aclient, i64_actorid aserver, std::string& asession)
	{
		std::string lkcpsession = std::format("{}&{}&{}", sysconfig::kcpsession(), aclient, aserver);
		asession = tools::md5(lkcpsession);
		log_error_net()->print("ukcp::session_create({}:{})", lkcpsession, asession);
		return true;
	}

	bool ukcp::session_check(i64_actorid aclient, i64_actorid aserver, const std::string& asession)
	{
		std::string lsession;
		if (session_create(aclient, aserver, lsession) == false)
		{
			return false;
		}
		return asession == lsession;
	}

	void ukcp::reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient)
	{
		m_kcp.reset_add(aconv, aip, aport, aserver, aclient);
	}

	void ukcp::reset_add(const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient)
	{
		m_kcp.reset_add(m_conv, aip, aport, aserver, aclient);
	}
}// namespace ngl