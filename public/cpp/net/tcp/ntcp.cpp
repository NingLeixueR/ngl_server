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

#include "actor_gateway.h"
#include "protocol.h"
#include "nguid.h"
#include "ntcp.h"

namespace ngl
{
	bool ntcp::socket_recv(service_io* ap, const char* abuff, int32_t abufflen)
	{
		if (abuff == nullptr && abufflen == 0)
		{// # 连接被异常中断 net -> application
			close(ap->m_sessionid);
			return true;
		}
		if (m_outernet == false)
		{// # 不允许外网访问
			if (ap->m_is_lanip == false)
			{//连接不是内网
				return false;
			}
		}
		return m_segpackvec[ap->m_threadid]->push(ap->m_sessionid, abuff, abufflen, ap->m_is_lanip);
	}

	bpool& ntcp::pool()
	{
		return m_pool;
	}

	bool ntcp::init(i16_port aport, i32_threadsize asocketthreadnum, bool aouternet)
	{
		if (asocketthreadnum > net_config_socket_pthread_max_size || asocketthreadnum <= 0)
		{
			asocketthreadnum = net_config_socket_pthread_max_size;
		}

		m_outernet = aouternet;
		m_port = aport;
		m_socketthreadnum = asocketthreadnum;

		for (int i = 0; i < asocketthreadnum; ++i)
		{
			m_segpackvec.push_back(std::make_shared<segpack>());
		}

		std::function<bool(service_io*, const char*, uint32_t)> lrecv = std::bind_front(&ntcp::socket_recv, this);

		std::function<void(i32_sessionid)> lclose = [this](i32_sessionid asession)
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

		std::function<void(i32_sessionid, bool, const pack*)> lsendfinish = [](i32_sessionid asessionid, bool abool, const pack*)
			{
				if (abool)
				{
					std::pair<str_servername, i32_serverid> lpair("none", -1);
					server_session::serverinfobysession(asessionid, lpair);
					log_error()->print("send finish fail sessionid:{} server[{}:{}]", asessionid, lpair.second, lpair.first);
				}
			};

		m_server = std::make_shared<asio_tcp>(port(), asocketthreadnum, lrecv, lclose, lsendfinish);
		return true;
	}

	i16_port ntcp::port()
	{
		return m_port;
	}

	void ntcp::close(i32_sessionid asession)
	{
		std::pair<str_servername, i32_serverid> lpair("none", -1);
		if (server_session::serverinfobysession(asession, lpair))
		{
			log_error()->print("close sessionid:{} server[{}:{}]", asession, lpair.second, lpair.first);
		}

		auto pro = std::make_shared<np_actor_session_close>();
		pro->m_sessionid = asession;
		i64_actorid lactorid = actor_gateway::actorid(nconfig.tcount());
		actor::send_actor(lactorid, nguid::make(), pro);

		server_session::remove(asession);
	}

	void ntcp::close_net(i32_sessionid asession)
	{
		m_server->close_net(asession);
	}

	void ntcp::set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun)
	{
		m_server->set_close(asession, [this, aip, aport, afun, asession]()
			{
				connect(aip, aport, afun);
			}
		);
	}

	bool ntcp::connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun)
	{
		m_server->connect(aip, aport, afun);
		return true;
	}

	bool ntcp::connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun, bool await, bool areconnection /*断线是否重连*/)
	{
		std::shared_ptr<ngl::sem> lsem = nullptr;
		if (await)
		{
			lsem = std::make_shared<ngl::sem>();
		}
		connect(aip, aport, [this, afun, aip, aport, areconnection, lsem](i32_sessionid asession)
			{
				afun(asession);
				if (lsem != nullptr)
				{
					lsem->post();
				}
				if (areconnection)
				{
					set_close(asession, aip, aport, afun);
				}
			}
		);
		if (lsem != nullptr)
		{
			lsem->wait();
		}
		return true;
	}

	const std::string& ntcp::ip(const net_works& anets)
	{
		return nconfig.nodetype() == ROBOT ? anets.m_ip : anets.m_nip;
	}

	bool ntcp::connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession != -1)
		{
			afun(lsession);
			return true;
		}

		net_works lnets;
		if (!ttab_servers::instance().connect(aserverid, lnets))
		{
			return false;
		}

		std::tuple<ENET_PROTOCOL, str_ip, i16_port> lpair = std::make_tuple(lnets.m_type, ip(lnets), lnets.m_port);

		log_info()->print("Connect Server {}@{}:{}", aserverid, std::get<1>(lpair), std::get<2>(lpair));
		return connect(std::get<1>(lpair), std::get<2>(lpair), [aserverid, afun](i32_session asession)
			{
				server_session::add(aserverid, asession);
				if (afun != nullptr)
				{
					afun(asession);
				}
			}, await, areconnection
		);
	}

	bool ntcp::send_msg(i32_sessionid asession, const std::string& amsg)
	{
		auto lcount = (int)amsg.size();
		auto lpack = std::make_shared<pack>();
		lpack->malloc(lcount + 1);
		memcpy(lpack->m_buff, amsg.c_str(), lcount);
		lpack->m_buff[lcount] = '\0';
		lpack->m_len = lcount + 1;
		lpack->m_pos = lcount + 1;
		return send_pack(asession, lpack);
	}

	bool ntcp::send_pack(i32_sessionid asession, std::shared_ptr<pack>& lpack)
	{
		return m_server->send(asession, lpack);
	}

	bool ntcp::send_pack(i32_sessionid asession, std::shared_ptr<void>& lpack)
	{
		return m_server->send(asession, lpack);
	}

	bool ntcp::send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_sessionid lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return send_pack(lsession, apack);
	}

	bool ntcp::send(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::shared_ptr<pack>& apack)
	{
		for (auto [_session, actorid] : asession)
		{
			apack->set_actor(actorid, aactorid);
			send_pack(_session, apack);
		}
		return  true;
	}

	bool ntcp::send(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::shared_ptr<pack>& apack)
	{
		for (i32_sessionid session : asession)
		{
			apack->set_actor(aactorid, arequestactorid);
			send_pack(session, apack);
		}
		return  true;
	}

	bool handle_pram::send_pack(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return ntcp::instance().send_pack(lsession, apack);
	}

	bool handle_pram::send_pack(i32_serverid aserverid, std::shared_ptr<void>& apack)
	{
		i32_session lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return ntcp::instance().send_pack(lsession, apack);
	}
}//namespace ngl