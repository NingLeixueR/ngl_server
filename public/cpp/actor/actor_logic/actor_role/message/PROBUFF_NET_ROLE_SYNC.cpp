#include "actor_role.h"
#include "ttab_task.h"
#include "drop.h"

namespace ngl
{
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata)
	{
		sync_data_client();
		log_error()->print("[sync]###[{}]", m_info.name());
		return true;
	}
}//namespace ngl