#include "actor_family.h"

namespace ngl
{
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->m_familid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>();
		int32_t lstat = m_family.change_familyname(lroleid, familid, adata.get_data()->data()->m_name());
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}
}//namespace ngl