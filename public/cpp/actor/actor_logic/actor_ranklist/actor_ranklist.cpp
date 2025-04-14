#include "actor_ranklist.h"
#include "actor_brief.h"

namespace ngl
{
	actor_ranklist::actor_ranklist() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_RANKLIST,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		std::set<i64_actorid> ldataid;
		tdb_brief::nsp_cli<actor_ranklist>::init(actor_brief::actor_type(), this, ldataid);
	}

	ENUM_ACTOR actor_ranklist::actor_type()
	{
		return ACTOR_RANKLIST;
	}

	i64_actorid actor_ranklist::actorid()
	{
		return nguid::make(ACTOR_RANKLIST, tab_self_area, nguid::none_actordataid());
	}

	void actor_ranklist::init()
	{
		m_ranklist.set(this);
	}

	void loaddb_finish(bool adbishave) 
	{
	}

	void actor_ranklist::nregister()
	{
		register_handle_custom<actor_ranklist>::func<
			mforward<np_gm>
		>(true);

		register_handle_proto<actor_ranklist>::func<
			mforward<pbnet::PROBUFF_NET_RANKLIST>
		>(true);
	}
}// namespace ngl