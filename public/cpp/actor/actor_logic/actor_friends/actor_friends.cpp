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

	void actor_friends::init()
	{
		m_friends.set(this);
	}

	void actor_friends::loaddb_finish(bool adbishave)
	{
		actor_events_logic::tfun::func<>(
			eevents_logic_rolelogin, actorid(tab_self_area),
			eevents_logic_roleoffline, actorid(tab_self_area)
		);
	}

	ENUM_ACTOR actor_friends::actor_type()
	{
		return ACTOR_FRIENDS;
	}

	i64_actorid actor_friends::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, nguid::none_actordataid());
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

		register_handle_custom<actor_friends>::func<
			np_eevents_logic_rolelogin
			, np_eevents_logic_roleoffline
		>(true);
		
	}
}// namespace ngl