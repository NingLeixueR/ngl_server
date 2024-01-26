#include "net.h"
#include "protocol.h"
#include "actor_guid.h"

namespace ngl
{
	net_protocol* nets::m_net;
	ENET_PROTOCOL nets::m_type;

	bool handle_pram::netsendpack(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		net_protocol* lpnet = nets::net();
		i32_sessionid lsession = lpnet->get_sessionid(aserverid);
		return nets::net()->sendpack(lsession, apack);
	}

	bool handle_pram::netsendpack(i32_serverid aserverid, std::shared_ptr<void>& apack)
	{
		net_protocol* lpnet = nets::net();
		i32_sessionid lsession = lpnet->get_sessionid(aserverid);
		return nets::net()->sendpack(lsession, apack);
	}

}//namespace ngl