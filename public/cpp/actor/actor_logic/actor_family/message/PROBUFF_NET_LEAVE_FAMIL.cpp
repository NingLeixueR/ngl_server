#include "actor_family.h"

namespace ngl
{
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_LEAVE_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->m_familid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>();
		int32_t lstat = m_family.leave_family(lroleid, familid);
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}
}//namespace ngl