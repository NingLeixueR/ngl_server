#include "actor_family.h"

namespace ngl
{
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid ljoinroleid = adata.get_data()->data()->m_roleid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE>();
		int32_t lstat = m_family.ratify_join_family(lroleid, ljoinroleid, adata.get_data()->data()->m_ratify());
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}
}//namespace ngl