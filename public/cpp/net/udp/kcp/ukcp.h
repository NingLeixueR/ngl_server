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
#pragma once

#include "net/udp/kcp/asio_kcp.h"
#include "net/net_pack.h"
#include "tools/type.h"

namespace ngl
{
	class ukcp
	{
		asio_kcp	m_kcp;
		bpool		m_pool;

	public:
		static const int32_t	m_conv = 1;
		static std::string		m_localuip;

		ukcp(i16_port aprot);

		static std::shared_ptr<ukcp> create(i16_port aprot);

#pragma region kcp_send
		template <typename T>
		bool send(const std::set<i64_actorid>& aactors, T& adata)
		{
			auto lpack = net_pack<T>::npack(&m_pool, adata, 0, 0);
			if (lpack == nullptr)
			{
				return false;
			}
			return send_server(aactors, lpack);
		}

		template <typename T>
		bool send(i64_actorid aactoridclient, T& adata)
		{
			std::set<i64_actorid> lactors = { aactoridclient };
			return send(lactors, adata);
		}

		bool send_server(std::shared_ptr<pack>& apack);

		bool send_server(const std::set<i64_actorid>& aactors, const std::shared_ptr<pack>& apack);

		bool send_server(i64_actorid aactor, const std::shared_ptr<pack>& apack);

		bool sendpackbyarea(i16_area aarea, const std::shared_ptr<pack>& apack);

		template <typename T>
		bool send(const asio_udp_endpoint& aendpoint, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			return m_kcp.send_server(aendpoint, lpack);
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

		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len);

		// ## 发送原始udp包并等待其返回
		bool sendu_waitrecv(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun);
#pragma endregion 

		// ## 发起连接
		void connect(
			std::string& akcpsess
			, i64_actorid aserver
			, i64_actorid aclient
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_session)>& afun
		);
		void connect(
			std::string& akcpsess
			, i64_actorid aserver
			, i64_actorid aclient
			, const asio_udp_endpoint& aendpoint
			, const std::function<void(i32_session)>& afun
		);

		// ## 查找session对应的actorid
		i64_actorid find_server(i32_session asession);
		i64_actorid find_client(i32_session asession);
		bool find_actorid(i32_session asession, i64_actorid& aserver, i64_actorid& aclient);

		// ## 生成kcp-session以验证连接
		static bool session_create(i64_actorid aclient, i64_actorid aserver, std::string& asession);

		// ## 检查kcp-session以验证连接
		static bool session_check(i64_actorid aclient, i64_actorid aserver, const std::string& asession);

		// ## 重置连接
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient);
		void reset_add(const std::string& aip, i16_port aport, i64_actorid aserver, i64_actorid aclient);
	};
}// namespace ngl