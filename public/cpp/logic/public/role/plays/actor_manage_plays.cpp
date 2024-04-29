#include "actor_manage_plays.h"
#include "actor_create.h"
#include "matching.h"

namespace ngl
{
	actor_manage_plays::actor_manage_plays() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MANAGE_PLAYS,
					.m_area = ttab_servers::tab()->m_area
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_manage_plays::nregister()
	{
		// ¶¨Ê±Æ÷
		register_timer<actor_manage_plays>(&actor_manage_plays::timer_handle);

		type_register_actor_handle<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_manage_plays>::func<
			pbnet::PROBUFF_NET_MATCHING_SUCCESS
		>(true);
	}

	void actor_manage_plays::loaddb_finish(bool adbishave)
	{
	}

	bool actor_manage_plays::handle(message<pbnet::PROBUFF_NET_MATCHING_SUCCESS>& adata)
	{
		auto lrecv = adata.m_data;
		int32_t tid = roomid::tid(lrecv->m_roomid());
		auto tab = allcsv::tab<tab_plays>(tid);
		assert(tab != nullptr);
		np_actorswitch_process_plays pram;
		std::for_each(lrecv->mutable_m_member()->begin(), lrecv->mutable_m_member()->end(), [&pram](const pbnet::MATCHING_MEMBER& adata)
			{
				pram.m_players.push_back(adata.m_id());
			});
		i64_actorid lactorid = nguid::make((ENUM_ACTOR)((int)ACTOR_PLAYS + (int)tab->m_type), ttab_servers::tab()->m_area, lrecv->m_roomid());
		actor_create::switch_process(lactorid, -1, xmlnode::m_nodeid, pram);
		return true;
	}

	bool actor_manage_plays::timer_handle(message<timerparm>& adata)
	{
		return true;
	}
}