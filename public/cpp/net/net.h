#pragma once

#include "net_tcp.h"
#include "net_ws.h"
#include "xmlnode.h"
#include "ttab_servers.h"

namespace ngl
{
	class nets
	{
		static net_protocol* m_net;
		static ENET_PROTOCOL m_type;
	public:
		static net_protocol* net()
		{
			return m_net;
		}

		static bool init(i16_port aport, i32_threadsize asocketthreadnum, bool aouternet)
		{
			//// #### 初始化服务器
			m_type = ttab_servers::netprotocol(nconfig::m_nodeid);
			switch (m_type)
			{
			case ENET_UDP:
			case ENET_TCP:
				m_net = new net_tcp();
				break;
			case ENET_WS:
				m_net = new net_ws();
				break;
			}
			m_net->init(aport, asocketthreadnum, aouternet);
			return true;
		}
	};

	template <typename T>
	bool handle_pram::netsend(i32_sessionid asession, T& adata, const actor_guid& aactorid, const actor_guid& arequestactorid)
	{
		return nets::net()->send(asession, adata, aactorid.id(), arequestactorid.id());
	}

	template <typename T>
	bool actor_base::send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nets::net()->send_server(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::sendpacktoserver(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		return nets::net()->sendpackbyserver(aserverid, apack);
	}

	template <typename T>
	bool actor_base::sendpackbysession(i32_sessionid asession, std::shared_ptr<pack>& apack)
	{
		return nets::net()->sendpack(asession, apack);
	}

	template <typename T>
	std::shared_ptr<pack> actor_base::net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ::ngl::net_pack<T>::npack(&nets::net()->get_pool(), adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nets::net()->send(asession, adata, aactorid, arequestactorid);
	}
}//namespace ngl

#define nserver (ngl::nets::net())
