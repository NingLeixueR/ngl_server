#include "actor_login.h"
#include "nregister.h"

namespace ngl
{
	bool actor_login::handle(const message<np_actorserver_connect>& adata)
	{
		auto lparm = adata.get_data();
		server_info ltemp
		{
			.m_id = lparm->m_serverid,
			.m_rolesize = 0
		};
		switch (ttab_servers::node_type(lparm->m_serverid))
		{
		case ngl::NODE_TYPE::GAME:
			m_game.insert(std::make_pair(lparm->m_serverid, ltemp));
			break;
		case ngl::NODE_TYPE::GATEWAY:
			m_gateway.insert(std::make_pair(lparm->m_serverid, ltemp));
			break;
		}
		log_error()->print("message<np_actorserver_connect>:{}", lparm->m_serverid);
		return true;
	}
	bool actor_login::handle(const message<np_actor_disconnect_close>& adata)
	{
		auto itor = m_actorbyserver.find(adata.get_data()->m_actorid);
		if (itor == m_actorbyserver.end())
		{
			return true;
		}
		dec_freeserver_game(itor->second.m_gameserverid);
		dec_freeserver_gateway(itor->second.m_gatewayserverid);
		m_actorbyserver.erase(itor);

		printf_freeserver();
		return true;
	}
}//namespace ngl