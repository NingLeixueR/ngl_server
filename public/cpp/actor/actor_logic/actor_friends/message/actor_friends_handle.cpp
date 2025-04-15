#include "actor_friends.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_friends::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		std::vector<i64_actorid> lfriends;
		if (m_friends.get_friends(adata.get_data()->m_actorid, lfriends))
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLESTAT>();
			pro->set_m_stat(pbnet::PROBUFF_NET_ROLESTAT::online);
			pro->set_m_logicstat(pbnet::PROBUFF_NET_ROLESTAT::friends);
			pro->set_m_roleid(adata.get_data()->m_actorid);
			send_client(lfriends, pro);
		}
		return true;
	}

	bool actor_friends::handle(const message<np_eevents_logic_roleoffline>& adata)
	{
		std::vector<i64_actorid> lfriends;
		if (m_friends.get_friends(adata.get_data()->m_actorid, lfriends))
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLESTAT>();
			pro->set_m_stat(pbnet::PROBUFF_NET_ROLESTAT::offline);
			pro->set_m_logicstat(pbnet::PROBUFF_NET_ROLESTAT::friends);
			pro->set_m_roleid(adata.get_data()->m_actorid);
			send_client(lfriends, pro);
		}
		return true;
	}
}//namespace ngl