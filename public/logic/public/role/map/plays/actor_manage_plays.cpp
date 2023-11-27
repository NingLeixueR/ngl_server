#include "actor_manage_plays.h"
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

	void actor_manage_plays::actor_register()
	{
		// ¶¨Ê±Æ÷
		register_timer<actor_manage_plays>(&actor_manage_plays::timer_handle);


	}

	void actor_manage_plays::loaddb_finish(bool adbishave)
	{
	}

	bool actor_manage_plays::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MATCHING_SUCCESS& adata)
	{
		adata.m_dataid();
		roomid ltemp(adata.m_roomid());
		int32_t tid = roomid::tid(adata.m_roomid());
		//ENUM_ACTOR atype, i32_actordataid aid, void* aparm = nullptr
		actor_switch_process_plays pram;
		std::for_each(adata.mutable_m_member()->begin(); adata.mutable_m_member()->end(); [](const pbnet::MATCHING_MEMBER& adata)
			{

			});
		pram.m_players;
		actor_base::create(ACTOR_PLAYS, adata.m_roomid(),);

		actor_switch_process_plays;
		return true;
	}

	bool actor_manage_plays::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		return true;
	}
}