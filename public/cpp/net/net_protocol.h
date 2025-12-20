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
#include "asio_kcp.h"
#include "segpack.h"
#include "pack.h"

#include <vector>
#include <memory>

namespace ngl
{
	class net_protocol
	{
		net_protocol() = delete;
		net_protocol(const net_protocol&) = delete;
		net_protocol& operator=(const net_protocol&) = delete;

	protected:
		i16_port				m_port = 0;											// 服务器监听端口号
		i32_threadsize			m_socketthreadnum = 0;								// socket 接收数据线程数
		bpool					m_pool;												// 发送池
		bool					m_outernet = false;									// 是否允许外网连接
		std::shared_mutex		m_mutex;											// 互斥量
		std::list<pack>			m_packlist;											// 发送队列
		ENET_PROTOCOL			m_protocol = ENET_NULL;								// 协议

		explicit net_protocol(ENET_PROTOCOL aprotocol);

		bool socket_recv(int asessionid, int aislanip, const char* abuff, int32_t abufflen);
	public:
		bpool& get_pool();

		// # 初始化net_protocol
		// # aport			i16_port		端口号
		// # athreadnum		i32_threadsize 	线程数
		// # aouternet		bool			是否允许非内网主动连接
		virtual bool init(i16_port aport, i32_threadsize athreadnum, bool aouternet);

		// # 关闭socket连接以及加载的数据
		// # 通知上层应用
		virtual void close(i32_sessionid asession);

		// # 逻辑层主动关闭连接(这样就不需要通知上层应用)
		virtual void close_net(i32_sessionid asession) = 0;

		// # 获取线程数量
		i32_threadsize socketthreadnum()const;

		// # 获取监听端口号
		i16_port port()const;

		// # 向某个服务器发送pack
		bool send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		// # 设置断线重连
		virtual void set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun) = 0;

		// # 尝试连接指定ip和端口
		virtual bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun) = 0;

		// # aip/aport		要连接的ip/端口
		// # afun			连接回调
		// # await			是否等待连接成功
		// # areconnection	断线是否重连
		virtual bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun, bool await, bool areconnection);

		// # 发送消息
		virtual bool send_pack(i32_sessionid asession, std::shared_ptr<pack>& lpack) = 0;
		virtual bool send_pack(i32_sessionid asession, std::shared_ptr<void>& lpack) = 0;

		// # 发送消息
		template <typename Y, typename T = Y>
		bool send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// # 向服务器发送消息
		template <typename Y, typename T = Y>
		bool send_server(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// # 给一组sesion发送消息
		// # key: session values:aactorid
		// # std::map<uint32_t, uint32_t>& asession
		void mores(i32_sessionid asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack);
		bool send(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::shared_ptr<pack>& apack);
		bool send(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack);

		// # 向客户端发送消息
		template <typename T>
		bool send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata);

		// # 向客户端发送消息
		template <typename T>
		void send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata);
	};
}// namespace ngl

namespace ngl
{
	template <typename Y, typename T/* = Y*/>
	bool net_protocol::send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		std::shared_ptr<pack> lpack = net_pack<T>::npack(&get_pool(), adata, aactorid, arequestactorid);
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

	// # 向服务器发送消息
	template <typename Y, typename T/* = Y*/>
	bool net_protocol::send_server(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		i32_sessionid lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return send<Y, T>(lsession, adata, aactorid, arequestactorid);
	}

	// # 向客户端发送消息
	template <typename T>
	bool net_protocol::send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata)
	{
		std::vector<std::pair<i32_actordataid, i16_area>> lvecs = { {auid, aarea} };
		return send_client(lvecs, agateway, adata);
	}

	// # 向客户端发送消息
	template <typename T>
	void net_protocol::send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata)
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
}