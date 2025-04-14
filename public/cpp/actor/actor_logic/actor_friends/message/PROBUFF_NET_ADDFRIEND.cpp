#include "actor_friends.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_ADDFRIEND>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lfriedid = adata.get_data()->data()->m_friedid();
		auto pro = std::make_shared<pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE>();
		int32_t lstat = m_friends.addfriends(lroleid, lfriedid);
		pro->set_m_friedid(lfriedid);
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}
}//namespace ngl