#include "actor_family.h"

namespace ngl
{
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_CREATE_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		auto pro = std::make_shared<pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE>();
		int32_t lstat = m_family.create_family(lroleid, adata.get_data()->data()->m_name());
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}
}//namespace ngl