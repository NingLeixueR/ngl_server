#include "actor_family.h"

namespace ngl
{
	actor_family::actor_family() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_FAMILY,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_family::actor_type()
	{
		return ACTOR_FAMILY;
	}

	i64_actorid actor_family::actorid(int32_t adataid)
	{
		return nguid::make(actor_type(), tab_self_area, adataid);
	}

	void actor_family::init()
	{
		// 绑定DB结构:DB.set(this);
		m_familyer.set(this);
		m_family.set(this);

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

	void actor_family::loaddb_finish(bool adbishave)
	{
		actor_events_logic::tfun::func<>(
			eevents_logic_rolelogin, actorid(tab_self_area),
			eevents_logic_roleoffline, actorid(tab_self_area)
		);
	}

	void actor_family::nregister()
	{
		// 定时器
		actor::register_timer<actor_family>(&actor_family::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_family>::func<
			mforward<np_gm>
			, np_eevents_logic_rolelogin
			, np_eevents_logic_roleoffline
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_family>::func<
			mforward<pbnet::PROBUFF_NET_CREATE_FAMIL>
			, mforward<pbnet::PROBUFF_NET_JOIN_FAMIL>
			, mforward<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>
			, mforward<pbnet::PROBUFF_NET_CEDE_FAMIL>
			, mforward<pbnet::PROBUFF_NET_LEAVE_FAMIL>
			, mforward<pbnet::PROBUFF_NET_FAMIL_LIST>
			, mforward<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>
			, mforward<pbnet::PROBUFF_NET_FAMILSIGN>
		>(true);
	}

	bool actor_family::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_family::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl