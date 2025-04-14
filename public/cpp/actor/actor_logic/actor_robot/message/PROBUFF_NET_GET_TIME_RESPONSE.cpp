#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata)
	{
		char lbuff[1024] = { 0 };
		ngl::localtime::time2str(lbuff, 1024, adata.get_data()->m_utc(), "%y/%m/%d %H:%M:%S");
		log_error()->print("[{}][{}]", m_data.m_role().m_base().m_name(), lbuff);
		return true;
	}
}//namespace ngl