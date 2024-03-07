#include "ukcp.h"


namespace ngl
{
	int32_t ukcp::m_conv = 1;
	std::string ukcp::m_localuip = "";

	ukcp::ukcp(i16_port aprot) :
		m_kcp(aprot)
	{}

	void ukcp::connect(const std::string& akcpsess
		, i64_actorid aactorid
		, const std::string& aip
		, i16_port aport
		, const std::function<void(i32_session)>& afun
	)
	{
		m_kcp.connect(m_conv++, akcpsess, aactorid, aip, aport, afun);
	}

	void ukcp::connect(const std::string& akcpsess
		, i64_actorid aactorid
		, const asio_udp_endpoint& aendpoint
		, const std::function<void(i32_session)>& afun
	)
	{
		m_kcp.connect(m_conv++, akcpsess, aactorid, aendpoint, afun);
	}

	i64_actorid ukcp::find_actorid(i32_session asession)
	{
		return m_kcp.find_actorid(asession);
	}

	// 生成kcp-session以验证连接
	bool ukcp::create_session(i64_actorid aactorid, std::string& asession)
	{
		xmlinfo* xml = nconfig::get_publicconfig();
		std::string lkcpsession;
		if (xml->find("kcp_session", lkcpsession) == false)
			return false;

		lkcpsession += '&';
		lkcpsession += boost::lexical_cast<std::string>(nguid::area(aactorid));

		lkcpsession += '&';
		lkcpsession += boost::lexical_cast<std::string>(nguid::actordataid(aactorid));

		md5 lmd5(lkcpsession);
		asession = lmd5.values();
		return true;
	}

	bool ukcp::check_session(i64_actorid aactorid, const std::string& asession)
	{
		std::string lsession;
		if (create_session(aactorid, lsession) == false)
			return false;
		return asession == lsession;
	}

	void ukcp::reset_add(int32_t aconv, const std::string& aip, i16_port aport)
	{
		m_kcp.reset_add(aconv, aip, aport);
	}

	void ukcp::reset_add(const std::string& aip, i16_port aport)
	{
		m_kcp.reset_add(m_conv++, aip, aport);
	}
}