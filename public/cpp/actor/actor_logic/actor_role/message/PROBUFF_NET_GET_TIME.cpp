#include "actor_role.h"
#include "ttab_task.h"
#include "drop.h"

namespace ngl
{
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_GET_TIME>& adata)
	{
		i64_actorid lrequest = adata.m_pack->m_head.get_request_actor();
		log_error()->print("{},NAME={}", guid(), m_info.get_constrole().m_base().m_name());
		if (adata.m_pack->m_protocol == ENET_KCP)
		{
			pbnet::PROBUFF_NET_GET_TIME_RESPONSE pro;
			pro.set_m_utc((int32_t)localtime::gettime());
			sendkcp(pro, lrequest);
		}
		else
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>();
			pro->set_m_utc((int32_t)localtime::gettime());
			send_client(id_guid(), pro);
		}
		return true;
	}
}//namespace ngl