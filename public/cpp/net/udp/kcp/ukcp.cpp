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
// File overview: Implements logic for kcp.


#include "actor/protocol/nprotocol.h"
#include "tools/tab/xml/sysconfig.h"
#include "actor/actor_base/core/nguid.h"
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
		bool lret = true;
		for (i64_actorid lactorid : aactors)
		{
			if (!send_server(lactorid, apack))
			{
				lret = false;
			}
		}
		return lret;
	}

	bool ukcp::send_server(i64_actorid aactor, const std::shared_ptr<pack>& apack)
	{
		i32_session lsession = m_kcp.find_session(aactor);
		if (lsession == -1)
		{
			return true;
		}
		i64_actorid lserver = 0;
		i64_actorid lclient = 0;
		if (!m_kcp.find_actorid(lsession, lserver, lclient))
		{
			return true;
		}
		const i64_actorid lactor = nconfig.nodetype() != ROBOT ? lclient : lserver;
		const i64_actorid lrequest = nconfig.nodetype() != ROBOT ? lserver : lclient;
		std::shared_ptr<pack> lpack = apack->clone_actor(lactor, lrequest);
		if (lpack == nullptr)
		{
			return false;
		}
		return m_kcp.send_server(lsession, lpack);
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
		std::unique_ptr<ngl::tools::sem> lsem = std::make_unique<ngl::tools::sem>();
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

	i32_session ukcp::find_session(i64_actorid aactorid)
	{
		return m_kcp.find_session(aactorid);
	}

	bool ukcp::find_actorid(i32_session asession, i64_actorid& aserver, i64_actorid& aclient)
	{
		return m_kcp.find_actorid(asession, aserver, aclient);
	}

	// kcp-session connection
	bool ukcp::session_create(i64_actorid aclient, i64_actorid aserver, std::string& asession)
	{
		// The logical session token is deterministic so both sides can validate the handshake.
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
