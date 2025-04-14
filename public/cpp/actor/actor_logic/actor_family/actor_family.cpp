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

	i64_actorid actor_family::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, nguid::none_actordataid());
	}

	void actor_family::init()
	{
		m_familyer.set(this);
		m_family.set(this);
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
		// Ð­Òé×¢²á
		register_handle_custom<actor_family>::func<
			mforward<np_gm>
			, np_eevents_logic_rolelogin
			, np_eevents_logic_roleoffline
		>(true);

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
}// namespace ngl