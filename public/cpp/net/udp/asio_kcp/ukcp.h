#pragma once

#include "asio_kcp.h"
#include "net_pack.h"
#include "type.h"

namespace ngl
{
	class ukcp
	{
		asio_kcp m_kcp;
		bpool m_pool;

		ukcp(i16_port aprot);
	public:
		static const int32_t m_conv = 1;
		static std::string m_localuip;

		static ukcp* create(i16_port aprot)
		{
			return new ukcp(aprot);
		}

#pragma region kcp_send
		template <typename T>
		bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			return m_kcp.sendpack(asession, lpack);
		}

		template <typename T>
		bool send(const asio_udp_endpoint& aendpoint, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			return m_kcp.sendpack(aendpoint, lpack);
		}
	private:
		template <typename T, typename TITOR>
		bool send(TITOR abeg, TITOR aend, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			for (; abeg != aend; ++abeg)
			{
				m_kcp.sendpack(*abeg, lpack);
			}
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
#pragma endregion 

#pragma region udp_send
		template <typename T>
		bool sendu(const asio_udp_endpoint& aendpoint, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			return m_kcp.sendu(aendpoint, lpack->m_buff, lpack->m_len);
		}

		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len)
		{
			return m_kcp.sendu(aendpoint, buf, len);
		}

		// ## ����ԭʼudp�����ȴ��䷵��
		bool sendu_waitrecv(
			const asio_udp_endpoint& aendpoint, 
			const char* buf, int len, 
			const std::function<void(char*, int)>& afun
		)
		{
			std::unique_ptr<ngl::sem> lsem(new ngl::sem());
			m_kcp.sendu_waitrecv(aendpoint, buf, len, [afun, &lsem](char* buff, int len)
				{
					afun(buff, len);
					lsem->post();
				});
			lsem->wait();
			return true;
		}
#pragma endregion 

		// ## ��������
		void connect(const std::string& akcpsess
			, i64_actorid aactorid
			, const std::string& aip, i16_port aport
			, const std::function<void(i32_session)>& afun
		);
		void connect(const std::string& akcpsess
			, i64_actorid aactorid
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		);

		// ## ����session��Ӧ��actorid
		i64_actorid find_actorid(i32_session asession);

		// ## ����kcp-session����֤����
		static bool create_session(i64_actorid aactorid, std::string& asession);

		// ## ���kcp-session����֤����
		static bool check_session(i64_actorid aactorid, const std::string& asession);

		// ## ��������
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport);

		void reset_add(const std::string& aip, i16_port aport);
	};
}// namespace ngl