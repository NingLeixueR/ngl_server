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
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		std::set<i64_actorid> ldataid;
		tdb_brief::nsp_cli<actor_friends>::init(actor_brief::actor_type(), this, ldataid);
	}

	ENUM_ACTOR actor_friends::actor_type()
	{
		return ACTOR_FRIENDS;
	}

	i64_actorid actor_friends::actorid(int32_t adataid)
	{
		return nguid::make(actor_type(), tab_self_area, adataid);
	}

	void actor_friends::init()
	{
		// 绑定DB结构:DB.set(this);
		m_friends.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_friends::loaddb_finish(bool adbishave)
	{
		actor_events_logic::tfun::func<>(
			eevents_logic_rolelogin, actorid(tab_self_area),
			eevents_logic_roleoffline, actorid(tab_self_area)
		);
	}

	void actor_friends::nregister()
	{
		// 定时器
		actor::register_timer<actor_friends>(&actor_friends::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_friends>::func<
			np_eevents_logic_rolelogin
			, np_eevents_logic_roleoffline
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_friends>::func<
			mforward<pbnet::PROBUFF_NET_FRIEND_ADD>
			, mforward<pbnet::PROBUFF_NET_FRIEND_ERASE>
			, mforward<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>
			, mforward<pbnet::PROBUFF_NET_FRIEND>
		>(true);
	}

	bool actor_friends::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_friends::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl