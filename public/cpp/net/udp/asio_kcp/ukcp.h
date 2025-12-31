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

#include "asio_kcp.h"
#include "net_pack.h"
#include "type.h"

namespace ngl
{
	class ukcp
	{
		asio_kcp	m_kcp;
		bpool		m_pool;

		ukcp(i16_port aprot);
	public:
		static const int32_t	m_conv = 1;
		static std::string		m_localuip;

		static ukcp* create(i16_port aprot)
		{
			return new ukcp(aprot);
		}

#pragma region kcp_send
		template <typename T>
		bool send(const std::set<i64_actorid>& aactors, T& adata)
		{
			std::shared_ptr<pack> lpack;
			lpack = net_pack<T>::npack(&m_pool, adata, 0, 0);
			return send_server(aactors, lpack);
		}

		template <typename T>
		bool send(i64_actorid aactoridclient, T& adata)
		{
			std::set<i64_actorid> lactors = { aactoridclient };
			return send(lactors, adata);
		}

		bool send_server(std::shared_ptr<pack>& apack)
		{
			return m_kcp.send_server(apack);
		}

		bool send_server(const std::set<i64_actorid>& aactors, std::shared_ptr<pack>& apack)
		{
			for (i64_actorid lactorcliend : aactors)
			{
				i32_session lsession = m_kcp.find_session(lactorcliend);
				if (lsession == -1)
				{
					continue;
				}
				i64_actorid lactoridserver = 0;
				i64_actorid lactoridclient = 0;
				if (!m_kcp.find_actorid(lsession, lactoridserver, lactoridclient))
				{
					continue;
				}
				if (nconfig.nodetype() != ROBOT)
				{
					pack_head::head_set_actor((int32_t*)apack->m_buff, lactoridclient, lactoridserver);
				}
				else
				{
					pack_head::head_set_actor((int32_t*)apack->m_buff, lactoridserver, lactoridclient);
				}
				m_kcp.send_server(lsession, apack);
			}
			return true;
		}

		bool send_server(i64_actorid aactor, std::shared_ptr<pack>& apack)
		{
			std::set<i64_actorid> lactors = { aactor };
			return send_server(lactors, apack);
		}

		bool sendpackbyarea(i16_area aarea, std::shared_ptr<pack>& apack)
		{
			return m_kcp.sendpackbyarea(aarea, apack);
		}

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

		bool sendu(const asio_udp_endpoint& aendpoint, const char* buf, int len)
		{
			return m_kcp.sendu(aendpoint, buf, len);
		}

		// ## 发送原始udp包并等待其返回
		bool sendu_waitrecv(const asio_udp_endpoint& aendpoint, const char* buf, int len, const std::function<void(char*, int)>& afun)
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

		// ## 发起连接
		void connect(std::string& akcpsess, i64_actorid aactoridserver, i64_actorid aactoridclient, const std::string& aip, i16_port aport, const std::function<void(i32_session)>& afun);
		void connect(std::string& akcpsess, i64_actorid aactoridserver, i64_actorid aactoridclient, const asio_udp_endpoint& aendpoint, const std::function<void(i32_session)>& afun);

		// ## 查找session对应的actorid
		i64_actorid find_actoridserver(i32_session asession);
		i64_actorid find_actoridclient(i32_session asession);
		bool find_actorid(i32_session asession, i64_actorid& aactoridserver, i64_actorid& aactoridclient);

		// ## 生成kcp-session以验证连接
		static bool session_create(i64_actorid aactoridclient, i64_actorid aactoridserver, std::string& asession);

		// ## 检查kcp-session以验证连接
		static bool session_check(i64_actorid aactoridclient, i64_actorid aactoridserver, const std::string& asession);

		// ## 重置连接
		void reset_add(int32_t aconv, const std::string& aip, i16_port aport, i64_actorid aactoridserver, i64_actorid aactoridclient);
		void reset_add(const std::string& aip, i16_port aport, i64_actorid aactoridserver, i64_actorid aactoridclient);
	};
}// namespace ngl