#include "actor_gateway.h"
#include "net_protocol.h"
#include "protocol.h"
#include "nguid.h"

namespace ngl
{
	net_protocol::net_protocol(int8_t aindex):
		m_index(aindex)
		, m_port(0x0)
		, m_socketthreadnum(0x0)
		, m_outernet(false)
	{
	}

	net_protocol::~net_protocol()
	{}

	bpool& net_protocol::get_pool()
	{
		return m_pool;
	}

	bool net_protocol::socket_recv(int asessionid, int aislanip, const char* abuff, int32_t abufflen)
	{
		if (abuff == nullptr && abufflen == 0)
		{// # 连接被异常中断 net -> application
			close(asessionid);
			return true;
		}
		if (m_outernet == false)
		{// # 不允许外网访问
			if (aislanip == false)
			{
				return false;//连接不是内网
			}
		}
		return true;
	}

	bool net_protocol::init(i16_port aport, i32_threadsize asocketthreadnum, bool aouternet)
	{
		m_outernet = aouternet;
		m_port = aport;
		m_socketthreadnum = asocketthreadnum;
		return true;
	}

	void net_protocol::close(i32_sessionid asession)
	{
		std::pair<str_servername, i32_serverid> lpair("none", -1);
		if (server_session::serverinfobysession(asession, lpair))
		{
			log_error()->print("close sessionid:{} server[{}:{}]", asession, lpair.second, lpair.first);
		}

		auto pro = std::make_shared<np_actor_session_close>();
		pro->m_sessionid = asession;
		i64_actorid lactorid = actor_gateway::actorid(nconfig::m_tcount);
		actor::send_actor(lactorid, nguid::make(), pro);

		server_session::remove(asession);
	}

	bool net_protocol::connect(const std::string& aip
		, i16_port aport
		, const std::function<void(i32_sessionid)>& afun
		, bool await
		, bool areconnection					// 断线是否重连
	)
	{
		std::shared_ptr<ngl::sem> lsem(await ? new ngl::sem() : nullptr);
		auto lfun = [this, afun, aip, aport, areconnection, lsem](i32_sessionid asession)
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
			};
		connect(aip, aport, lfun);
		if (lsem != nullptr)
		{
			lsem->wait();
		}
		return true;
	}

	i32_threadsize net_protocol::socketthreadnum()
	{
		return m_socketthreadnum;
	}

	i16_port net_protocol::port()
	{
		return m_port;
	}

	bool net_protocol::sendpack(i32_sessionid asession, std::shared_ptr<pack>& apack)
	{
		return net_send(asession, apack);
	}

	bool net_protocol::sendpack(i32_sessionid asession, std::shared_ptr<void>& apack)
	{
		return net_send(asession, apack);
	}

	bool net_protocol::sendpackbyserver(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_sessionid lsession = server_session::sessionid(aserverid);
		if (lsession == -1)
		{
			return false;
		}
		return net_send(lsession, apack);
	}

	bool net_protocol::sendmore(
		const std::map<i32_sessionid, i64_actorid>& asession, 
		i64_actorid aactorid, 
		std::shared_ptr<pack>& apack
	)
	{
		for (auto& item : asession)
		{
			if (nets::session2type(item.first) != (ENET_PROTOCOL)m_index)
			{
				continue;
			}
			apack->set_actor(item.second, aactorid);
			net_send(item.first, apack);
		}
		return  true;
	}

	bool net_protocol::sendmore(
		const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid,
		std::shared_ptr<pack>& apack
	)
	{
		for (i32_sessionid item : asession)
		{
			if (nets::session2type(item) != (ENET_PROTOCOL)m_index)
			{
				continue;
			}
			apack->set_actor(aactorid, arequestactorid);
			net_send(item, apack);
		}
		return  true;
	}
}// namespace ngl