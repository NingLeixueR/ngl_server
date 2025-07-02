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
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_ADD>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lfriedid = adata.get_data()->data()->mfriedid();
		auto pro = std::make_shared<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>();
		int32_t lstat = m_friends.addfriends(lroleid, lfriedid);
		pro->set_mfriedid(lfriedid);
		pro->set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_ERASE>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lfriedid = adata.get_data()->data()->mfriedid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>();
		int32_t lstat = m_friends.erasefriends(lroleid, lfriedid);
		pro->set_mfriedid(lfriedid);
		pro->set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lfriedid = adata.get_data()->data()->mfriedid();
		bool lratify = adata.get_data()->data()->mratify();

		auto pro = std::make_shared<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>();
		int32_t lstat = m_friends.ratifyfriends(lroleid, lfriedid, lratify);
		pro->set_mfriedid(lfriedid);
		pro->set_mratify(lratify);
		pro->set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_friends::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		std::vector<i64_actorid> lfriends;
		if (m_friends.get_friends(adata.get_data()->m_actorid, lfriends))
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLESTAT>();
			pro->set_mstat(pbnet::PROBUFF_NET_ROLESTAT::online);
			pro->set_mlogicstat(pbnet::PROBUFF_NET_ROLESTAT::friends);
			pro->set_mroleid(adata.get_data()->m_actorid);
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
			pro->set_mstat(pbnet::PROBUFF_NET_ROLESTAT::offline);
			pro->set_mlogicstat(pbnet::PROBUFF_NET_ROLESTAT::friends);
			pro->set_mroleid(adata.get_data()->m_actorid);
			send_client(lfriends, pro);
		}
		return true;
	}
}//namespace ngl
