#include "actor_friends.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		m_friends.syncfriends(lroleid);
		return true;
	}
}//namespace ngl