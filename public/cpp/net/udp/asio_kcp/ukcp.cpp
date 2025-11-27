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
#include "sysconfig.h"
#include "nguid.h"
#include "ukcp.h"

namespace ngl
{
	std::string ukcp::m_localuip = "";

	ukcp::ukcp(i16_port aprot) :
		m_kcp(aprot)
	{}

	void ukcp::connect(std::string& akcpsess
		, i64_actorid aactoridserver
		, i64_actorid aactoridclient
		, const std::string& aip
		, i16_port aport
		, const std::function<void(i32_session)>& afun
	)
	{
		m_kcp.connect(m_conv, akcpsess, aactoridserver, aactoridclient, aip, aport, afun);
	}

	void ukcp::connect(std::string& akcpsess
		, i64_actorid aactoridserver
		, i64_actorid aactoridclient
		, const asio_udp_endpoint& aendpoint
		, const std::function<void(i32_session)>& afun
	)
	{
		m_kcp.connect(m_conv, akcpsess, aactoridserver, aactoridclient, aendpoint, afun);
	}

	i64_actorid ukcp::find_actoridserver(i32_session asession)
	{
		return m_kcp.find_actoridserver(asession);
	}

	i64_actorid ukcp::find_actoridclient(i32_session asession)
	{
		return m_kcp.find_actoridclient(asession);
	}

	bool ukcp::find_actorid(i32_session asession, i64_actorid& aactoridserver, i64_actorid& aactoridclient)
	{
		return m_kcp.find_actorid(asession, aactoridserver, aactoridclient);
	}

	// 生成kcp-session以验证连接
	bool ukcp::create_session(i64_actorid aactoridclient, i64_actorid aactoridserver, std::string& asession)
	{
		std::string lkcpsession = std::format("{}&{}&{}", sysconfig::kcpsession(), aactoridclient, aactoridserver);
		asession = tools::md5(lkcpsession);
		log_error_net()->print("ukcp::create_session({}:{})", lkcpsession, asession);
		return true;
	}

	bool ukcp::check_session(i64_actorid aactoridclient, i64_actorid aactoridserver, const std::string& asession)
	{
		std::string lsession;
		if (create_session(aactoridclient, aactoridserver, lsession) == false)
		{
			return false;
		}
		return asession == lsession;
	}

	void ukcp::reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aactoridserver, i64_actorid aactoridclient)
	{
		m_kcp.reset_add(aconv, aip, aport, aactoridserver, aactoridclient);
	}

	void ukcp::reset_add(const std::string& aip, i16_port aport, i64_actorid aactoridserver, i64_actorid aactoridclient)
	{
		m_kcp.reset_add(m_conv, aip, aport, aactoridserver, aactoridclient);
	}
}// namespace ngl