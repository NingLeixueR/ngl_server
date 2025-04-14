#include "actor_notice.h"

namespace ngl
{
	actor_notice::actor_notice() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_NOTICE,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_notice::actor_type()
	{
		return ACTOR_NOTICE;
	}

	i64_actorid actor_notice::actorid()
	{
		return nguid::make(ACTOR_NOTICE, tab_self_area, nguid::none_actordataid());
	}

	void actor_notice::init()
	{
		m_notice.set(this);
	}

	void actor_notice::loaddb_finish(bool adbishave) 
	{
	}

	bool actor_notice::sendnotice(const std::string& anotice, int32_t astarttime, int32_t afinishtime)
	{
		auto pro = std::make_shared<np_actor_addnotice>();
		pro->m_notice = anotice;
		pro->m_starttime = astarttime;
		pro->m_finishtime = afinishtime;
		actor::static_send_actor(actorid(), nguid::make(), pro);
		return true;
	}

	void actor_notice::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_notice>::func<
			mforward<np_gm>
		>(true);

		register_handle_proto<actor_notice>::func<
			mforward<pbnet::PROBUFF_NET_NOTICE>
		>(true);
	}
}//namespace ngl