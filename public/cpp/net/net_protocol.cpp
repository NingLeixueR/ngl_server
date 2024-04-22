
#include "actor_gateway.h"
#include "net_protocol.h"
#include "protocol.h"
#include "nguid.h"

namespace ngl
{
	struct net_protocol::impl_net_protocol
	{
		i16_port				m_port;											// 服务器监听端口号
		i32_threadsize			m_socketthreadnum;								// socket 接收数据线程数
		bpool					m_pool;
		bool					m_outernet;									    // 是否允许外网连接
		std::shared_mutex		m_mutex;
		std::list<pack>			m_packlist;

		impl_net_protocol():
			m_port(0x0),
			m_socketthreadnum(0x0),
			m_outernet(false)
		{}

		inline bool socket_recv(int asessionid, int aislanip, const char* abuff, uint32_t abufflen)
		{
			if (abuff == nullptr && abufflen == 0)
			{//// --- 连接被异常中断 net -> application
				close(asessionid);
				return true;
			}
			if (m_outernet == false)
			{// 不允许外网访问
				if (aislanip == false)
					return false;//连接不是内网
			}
			return true;
		}

		inline bool init(i16_port aport, i32_threadsize asocketthreadnum, bool aouternet)
		{
			m_outernet = aouternet;
			m_port = aport;
			m_socketthreadnum = asocketthreadnum;
			return true;
		}

		inline void close(i32_sessionid asession)
		{
			std::shared_ptr<np_actor_session_close> pro(
				new np_actor_session_close
				{ 
					.m_sessionid = asession 
				}
			);
			i64_actorid lactorid = actor_gateway::actorid(nconfig::m_nodeid);
			actor_base::static_send_actor(lactorid, nguid::make(), pro);

			server_session::remove(asession);
		}

		inline bool connect(
			net_protocol* anetprotocol
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_sessionid)>& afun
			, bool await
			, bool areconnection					// 断线是否重连
		)
		{
			boost::shared_ptr<ngl::sem> lsem(await ? new ngl::sem() : nullptr);
			anetprotocol->connect(aip, aport, [this, anetprotocol, afun, aip, aport, areconnection, lsem](i32_sessionid asession)
				{
					afun(asession);
					if (lsem != nullptr)
						lsem->post();
					if (areconnection)
						anetprotocol->set_close(asession, aip, aport, afun);
				});
			if (lsem != nullptr)
				lsem->wait();
			return true;
		}
	};

	net_protocol::net_protocol(int8_t aindex):
		m_index(aindex)
	{
		m_impl_net_protocol.make_unique();
	}

	net_protocol::~net_protocol()
	{}

	bpool& net_protocol::get_pool()
	{
		return m_impl_net_protocol()->m_pool;
	}

	bool net_protocol::socket_recv(int asessionid, int aislanip, const char* abuff, uint32_t abufflen)
	{
		return m_impl_net_protocol()->socket_recv(asessionid, aislanip, abuff, abufflen);
	}

	bool net_protocol::init(i16_port aport, i32_threadsize asocketthreadnum, bool aouternet)
	{
		return m_impl_net_protocol()->init(aport, asocketthreadnum, aouternet);
	}

	void net_protocol::close(i32_sessionid asession)
	{
		m_impl_net_protocol()->close(asession);
	}

	bool net_protocol::connect(const std::string& aip
		, i16_port aport
		, const std::function<void(i32_sessionid)>& afun
		, bool await
		, bool areconnection					// 断线是否重连
	)
	{
		return m_impl_net_protocol()->connect(this, aip, aport, afun, await, areconnection);
	}

	int net_protocol::socketthreadnum()
	{
		return m_impl_net_protocol()->m_socketthreadnum;
	}

	int net_protocol::port()
	{
		return m_impl_net_protocol()->m_port;
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
		i32_sessionid lsession = server_session::get_sessionid(aserverid);
		if (lsession == -1)
			return false;
		return net_send(lsession, apack);
	}

	bool net_protocol::sendmore(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>>& apair)
	{
		for (auto& item : asession)
		{
			if (nets::session2type(item.first) != (ENET_PROTOCOL)m_index)
				continue;
			apair.first->set_actor(item.second, aactorid);
			net_send(item.first, apair.first);
			if (apair.second != nullptr)
				net_send(item.first, apair.second);
		}
		return  true;
	}
}// namespace ngl