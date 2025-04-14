#include "actor_family.h"

namespace ngl
{
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_JOIN_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->m_familid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>();
		int32_t lstat = 0;
		if (adata.get_data()->data()->m_apply())
		{
			lstat = m_family.join_family(lroleid, familid);
		}
		else
		{
			lstat = m_family.cancel_join_family(lroleid, familid);
		}

		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}
}//namespace ngl