#include "actor_friends.h"
#include "actor_brief.h"

namespace ngl
{
	actor_friends::actor_friends() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_FRIENDS,
					.m_area = ttab_servers::tab()->m_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		std::set<i64_actorid> ldataid;
		tdb_brief::nsp_cli<actor_friends>::init(actor_brief::actor_type(), this, ldataid);
	}

	void actor_friends::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_proto<actor_friends>::func<
			mforward<pbnet::PROBUFF_NET_ADDFRIEND>
			, mforward<pbnet::PROBUFF_NET_ERASEFRIEND>
			, mforward<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND>
			, mforward<pbnet::PROBUFF_NET_FRIEND>
		>(true);
	}

	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		m_friends.syncfriends(lroleid);
		return true;
	}

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

	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lfriedid = adata.get_data()->data()->m_friedid();
		bool lratify = adata.get_data()->data()->m_ratify();

		auto pro = std::make_shared<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE>();
		int32_t lstat = m_friends.ratifyfriends(lroleid, lfriedid, lratify);
		pro->set_m_friedid(lfriedid);
		pro->set_m_ratify(lratify);
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}

	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_ERASEFRIEND>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lfriedid = adata.get_data()->data()->m_friedid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE>();
		int32_t lstat = m_friends.erasefriends(lroleid, lfriedid);
		pro->set_m_friedid(lfriedid);
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}
}// namespace ngl