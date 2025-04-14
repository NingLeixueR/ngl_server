#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata)
	{
		Try
		{
			log_error()->print("[LOGIC_ROLE_SYNC:{}:{}]"
				, adata.get_data()->m_role().m_base().m_name()
				, adata.get_data()->m_role().m_base().m_lv()
			);
			m_data = *adata.get_data();
		}Catch
			return true;
	}
}//namespace ngl