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

#include "server_session.h"
#include "netbuff_pool.h"
#include "handle_pram.h"
#include "structbytes.h"
#include "actor_base.h"
#include "net_pack.h"
#include "asio_tcp.h"
#include "segpack.h"
#include "pack.h"

#include <vector>
#include <memory>


namespace ngl
{
	class ntcp
	{
		std::shared_ptr<asio_tcp>				m_server = nullptr;
		std::vector<std::shared_ptr<segpack>>	m_segpackvec;

		i16_port				m_port = 0;											// 服务器监听端口号
		i32_threadsize			m_socketthreadnum = 0;								// socket 接收数据线程数
		bool					m_outernet = false;									// 是否允许外网连接
		bpool					m_pool;												// 发送池
		std::shared_mutex		m_mutex;											// 互斥量
		std::list<pack>			m_packlist;											// 发送队列
	private:
		bool socket_recv(service_io* ap, const char* abuff, int32_t abufflen);
	public:
		static ntcp& instance()
		{
			static ntcp ltemp;
			return ltemp;
		}

		bpool& pool()
		{
			return m_pool;
		}

		// # 监听端口 线程数量
		bool init(i16_port aport, i32_threadsize asocketthreadnum, bool	aouternet);

		i16_port port();

		// # 关闭socket连接以及加载的数据
		// # 通知上层应用
		void close(i32_sessionid asession);

		// # 关闭session
		void close_net(i32_sessionid asession);

		// # 设置socket关闭后的断线重连,afun:connect的parm3
		void set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);

		// # 连接ip:aport
		bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);

		// # aip/aport		要连接的ip/端口
		// # afun			连接回调
		// # await			是否等待连接成功
		// # areconnection	断线是否重连
		bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun, bool await, bool areconnection);

		bool connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection);

		bool send_msg(i32_sessionid asession, const std::string& amsg);

		// # 发送消息
		bool send_pack(i32_sessionid asession, std::shared_ptr<pack>& lpack);

		// # 发送消息
		bool send_pack(i32_sessionid asession, std::shared_ptr<void>& lpack);

		// # 向某个服务器发送pack
		bool send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		// # 发送消息
		template <typename Y, typename T = Y>
		bool send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&m_pool, adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			if (send_pack(asession, lpack) == false)
			{
				return false;
			}
			return true;
		}

		template <typename Y, typename T = Y>
		bool send(const std::map<i32_sessionid, i64_actorid>& asession, const Y& adata, i64_actorid aactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&pool(), adata, aactorid, 0);
			if (lpack == nullptr)
			{
				return false;
			}
			return send(asession, aactorid, lpack);
		}

		template <typename Y, typename T = Y>
		bool send(const std::set<i32_sessionid>& asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&pool(), adata, aactorid, 0);
			if (lpack == nullptr)
			{
				return false;
			}
			return send(asession, aactorid, arequestactorid, lpack);
		}

		// # 向服务器发送消息
		template <typename Y, typename T = Y>
		bool send_server(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			i32_sessionid lsession = server_session::sessionid(aserverid);
			if (lsession == -1)
			{
				return false;
			}
			return send<Y, T>(lsession, adata, aactorid, arequestactorid);
		}

		template <typename Y, typename T = Y>
		bool send_server(const std::set<i32_serverid>& aserverids, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::set<i32_session> lsessions;
			for (i32_serverid iserverid : aserverids)
			{
				i32_session lsession = server_session::sessionid(iserverid);
				if (lsession != -1)
				{
					lsessions.insert(lsession);
				}
			}
			if (!lsessions.empty())
			{
				return send<Y, T>(lsessions, adata, aactorid, arequestactorid);
			}
			return false;
		}

		// # 给一组sesion发送消息
		bool send(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::shared_ptr<pack>& apack);
		bool send(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack);

		// # 向客户端发送消息
		template <typename T>
		bool send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata)
		{
			std::vector<std::pair<i32_actordataid, i16_area>> lvecs = { {auid, aarea} };
			return send_client(lvecs, agateway, adata);
		}

		// # 向客户端发送消息
		template <typename T>
		void send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata)
		{
			np_actor_forward<T, forward_g2c<forward>> pro;
			for (int i = 0; i < avec.size(); ++i)
			{
				pro.m_data.m_uid.push_back(avec[i].first);
				pro.m_data.m_area.push_back(avec[i].second);
			}

			forward& lforward = pro.m_data.m_data;

			ngl::ser::serialize_byte lserializebyte;
			ngl::ser::nserialize::bytes(&lserializebyte, adata);

			lforward.m_bufflen = lserializebyte.pos();
			lforward.m_buff = netbuff_pool::instance().malloc_private(lforward.m_bufflen);

			ngl::ser::serialize_push lserializepush(lforward.m_buff, lforward.m_bufflen);
			if (ngl::ser::nserialize::push(&lserializepush, adata))
			{
				if (agateway != 0)
				{
					i32_session lsession = server_session::sessionid(agateway);
					if (lsession > 0)
					{
						send(lsession, pro, nguid::make(), nguid::make());
						netbuff_pool::instance().free((char*)lforward.m_buff);
						return;
					}
				}
			}
			netbuff_pool::instance().free((char*)lforward.m_buff);
		}
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool handle_send<T>::send_server(i32_serverid aserverid, const handle_pram& adata)
	{
		return ntcp::instance().send_server<T>(aserverid, *(T*)adata.m_data.get(), adata.m_actor, adata.m_requestactor);
	}

	template <typename T>
	bool handle_send<T>::send_server(i32_serverid aserverid, const nguid& aactorid, const nguid& arequestactorid, const T& adata)
	{
		return ntcp::instance().send_server<T>(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ntcp::instance().send_server<T>(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send_server(const std::set<i32_serverid>& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ntcp::instance().send_server<T>(aserverids, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool handle_pram::send(i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid)
	{
		return ntcp::instance().send(asession, adata, aactorid.id(), arequestactorid.id());
	}

	template <typename T>
	std::shared_ptr<pack> actor_base::net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ::ngl::net_pack<T>::npack(&ntcp::instance().pool(), adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ntcp::instance().send(asession, adata, aactorid, arequestactorid);
	}
}//namespace ngl