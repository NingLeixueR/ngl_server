#include "protocol.h"
#include "nguid.h"
#include "net.h"

namespace ngl
{
	std::array<net_protocol*, ENET_COUNT> nets::m_net;

	bool handle_pram::netsendpack(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		i32_session lsession = msession::sessionid(aserverid);
		if (lsession == -1)
			return false;
		return nets::net(lsession)->sendpack(lsession, apack);
	}

	bool handle_pram::netsendpack(i32_serverid aserverid, std::shared_ptr<void>& apack)
	{
		i32_session lsession = msession::sessionid(aserverid);
		if (lsession == -1)
			return false;
		return nets::net(lsession)->sendpack(lsession, apack);
	}
}//namespace ngl