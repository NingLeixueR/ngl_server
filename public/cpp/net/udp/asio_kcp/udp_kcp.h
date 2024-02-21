#pragma once

#include "asio_kcp.h"
#include "type.h"
#include "net_protocol.h"
#include "md5.h"

namespace ngl
{
	class udp_kcp
	{
		asio_kcp m_kcp;
		bpool m_pool;


		udp_kcp(i16_port aprot) :
			m_kcp(aprot)
		{}
	public:
		static udp_kcp& getInstance(i16_port aprot = -1)
		{
			static bool lfirst = true;
			if (lfirst && aprot == -1)
				throw "udp_kcp::getInstance(-1) first";
			lfirst = false;
			static udp_kcp ltemp(aprot);
			return ltemp;
		}


		template <typename T>
		bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
				return false;
			return m_kcp.sendpack(asession, lpack);
		}

		template <typename T>
		bool send(const asio_udp_endpoint& aendpoint, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
				return false;
			return m_kcp.sendpack(aendpoint, lpack);
		}

	private:
		template <typename T, typename TITOR>
		bool send(TITOR abeg, TITOR aend, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
				return false;
			for (; abeg != aend; ++abeg)
				m_kcp.sendpack(*abeg, lpack);
			return true;
		}
	public:

		template <typename T>
		bool send(std::vector<i32_sessionid> avec, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			return send(avec.begin(), avec.end(), adata, aactorid, arequestactorid);
		}

		template <typename T>
		bool send(std::list<i32_sessionid> avec, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			return send(avec.begin(), avec.end(), adata, aactorid, arequestactorid);
		}

		template <typename T>
		bool send(std::set<i32_sessionid> avec, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			return send(avec.begin(), avec.end(), adata, aactorid, arequestactorid);
		}
		
		void connect(const std::string& akcpsess
			, i64_actorid aactorid
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		)
		{
			m_kcp.connect(akcpsess, aactorid, aip, aport, afun);
		}

		void connect(const std::string& akcpsess
			, i64_actorid aactorid
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		)
		{
			m_kcp.connect(akcpsess, aactorid, aendpoint, afun);
		}

		i64_actorid find_actorid(i32_session asession)
		{
			return m_kcp.find_actorid(asession);
		}

		// 生成kcp-session以验证连接
		static bool create_session(i64_actorid aactorid, std::string& asession)
		{
			xmlinfo* xml = nconfig::get_publicconfig();
			std::string lkcpsession;
			if (xml->find("kcp_session", lkcpsession) == false)
				return false;

			lkcpsession += '&';
			lkcpsession += boost::lexical_cast<std::string>(actor_guid::area(aactorid));
			
			lkcpsession += '&';
			lkcpsession += boost::lexical_cast<std::string>(actor_guid::actordataid(aactorid));

			md5 lmd5(lkcpsession);
			asession = lmd5.values();
			return true;
		}

		static bool check_session(i64_actorid aactorid, const std::string& asession)
		{
			std::string lsession;
			if (create_session(aactorid, lsession) == false)
				return false;
			return asession == lsession;
		}
	};
}