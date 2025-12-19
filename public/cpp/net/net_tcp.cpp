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
#include "protocol.h"
#include "net_tcp.h"
#include "nguid.h"
#include "xml.h"

namespace ngl
{
	bool net_tcp::socket_recv(service_io* ap, const char* abuff, int32_t abufflen)
	{
		if (!net_protocol::socket_recv(ap->m_sessionid, ap->m_is_lanip, abuff, abufflen))
		{
			return false;
		}
		return m_segpackvec[ap->m_threadid]->push(ap->m_sessionid, abuff, abufflen, ap->m_is_lanip);
	}

	bool net_tcp::init(i16_port aport, i32_threadsize asocketthreadnum, bool aouternet)
	{
		if (asocketthreadnum > net_config_socket_pthread_max_size || asocketthreadnum <= 0)
		{
			asocketthreadnum = net_config_socket_pthread_max_size;
		}

		net_protocol::init(aport, asocketthreadnum, aouternet);

		for (int i = 0; i < asocketthreadnum; ++i)
		{
			m_segpackvec.push_back(std::make_shared<segpack>());
		}

		std::function<bool(service_io*, const char*, uint32_t)> lfun = std::bind_front(&net_tcp::socket_recv, this);

		std::function<void(i32_sessionid)> lclosefun = [this](i32_sessionid asession)
			{
				// 如果断开连接的是db服务器 
				// 自动关闭此进程
				// 防止玩家数据无法保存造成回档
				i32_serverid lserverid = server_session::serverid(asession);
				if (lserverid != -1 && ttab_servers::instance().node_type(nnodeid::tid(lserverid)) == NODE_TYPE::DB)
				{
					tools::no_core_dump();
				}
				close(asession);
			};
		m_server = std::make_shared<asio_tcp>(m_protocol, port(), asocketthreadnum, lfun, lclosefun, [](i32_sessionid asessionid, bool abool, const pack*)
			{
				if (abool)
				{
					std::pair<str_servername, i32_serverid> lpair("none", -1);
					server_session::serverinfobysession(asessionid, lpair);
					log_error()->print("send finish fail sessionid:{} server[{}:{}]", asessionid, lpair.second, lpair.first);
				}
			}
		);
		return true;
	}

	bool net_tcp::connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun)
	{
		m_server->connect(aip, aport, afun);
		return true;
	}

	void net_tcp::close_net(i32_sessionid asession)
	{
		m_server->close_net(asession);
	}

	void net_tcp::set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun)
	{
		m_server->set_close(asession, [this, aip, aport, afun, asession]()
			{
				connect(aip, aport, afun);
			});
	}

	bool net_tcp::send_pack(i32_sessionid asession, std::shared_ptr<pack>& lpack)
	{
		return m_server->send(asession, lpack);
	}

	bool net_tcp::send_pack(i32_sessionid asession, std::shared_ptr<void>& lpack)
	{
		return m_server->send(asession, lpack);
	}

	bool net_tcp::exist_session(i32_sessionid asession)
	{
		return m_server->exist_session(asession);
	}
}//namespace ngl