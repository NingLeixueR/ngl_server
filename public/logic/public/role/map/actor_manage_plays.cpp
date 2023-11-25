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
					.m_type = ACTOR_PLAYS,
					.m_area = ttab_servers::tab()->m_area
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_manage_plays::actor_register()
	{
		// ¶¨Ê±Æ÷
		actor_manage_plays::register_timer<actor_manage_plays>(&actor_manage_plays::timer_handle);


	}

	void actor_manage_plays::loaddb_finish(bool adbishave)
	{
	}

	bool actor_manage_plays::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MATCHING_SUCCESS& adata)
	{
		adata.m_dataid();
		roomid ltemp(adata.m_roomid());
		pbnet::eplays ltype = roomid::type(adata.m_roomid());
		//ENUM_ACTOR atype, i32_actordataid aid, void* aparm = nullptr
		//actor_base::create(ACTOR_PLAYS, );
	}

	bool actor_manage_plays::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		return true;
	}
}