/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
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
		, i64_actorid aactorid
		, const std::string& aip
		, i16_port aport
		, const std::function<void(i32_session)>& afun
	)
	{
		m_kcp.connect(m_conv, akcpsess, aactorid, aip, aport, afun);
	}

	void ukcp::connect(std::string& akcpsess
		, i64_actorid aactorid
		, const asio_udp_endpoint& aendpoint
		, const std::function<void(i32_session)>& afun
	)
	{
		m_kcp.connect(m_conv, akcpsess, aactorid, aendpoint, afun);
	}

	i64_actorid ukcp::find_actorid(i32_session asession)
	{
		return m_kcp.find_actorid(asession);
	}

	// ����kcp-session����֤����
	bool ukcp::create_session(i64_actorid aactorid, std::string& asession)
	{
		std::string lkcpsession = std::format("{}&{}&{}", sysconfig::kcpsession(), nguid::area(aactorid), nguid::actordataid(aactorid));
		asession = tools::md5(lkcpsession);
		return true;
	}

	bool ukcp::check_session(i64_actorid aactorid, const std::string& asession)
	{
		std::string lsession;
		if (create_session(aactorid, lsession) == false)
		{
			return false;
		}
		return asession == lsession;
	}

	void ukcp::reset_add(int32_t aconv, const std::string& aip, i16_port aport)
	{
		m_kcp.reset_add(aconv, aip, aport);
	}

	void ukcp::reset_add(const std::string& aip, i16_port aport)
	{
		m_kcp.reset_add(m_conv, aip, aport);
	}
}// namespace ngl