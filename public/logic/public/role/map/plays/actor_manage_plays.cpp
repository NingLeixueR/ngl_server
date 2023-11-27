#include "actor_manage_plays.h"
#include "matching.h"
#include "actor_create.h"

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
		// ��ʱ��
		register_timer<actor_manage_plays>(&actor_manage_plays::timer_handle);


	}

	void actor_manage_plays::loaddb_finish(bool adbishave)
	{
	}

	bool actor_manage_plays::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MATCHING_SUCCESS& adata)
	{
		roomid ltemp(adata.m_roomid());
		int32_t tid = roomid::tid(adata.m_roomid());
		//ENUM_ACTOR atype, i32_actordataid aid, void* aparm = nullptr

		auto tab = allcsv::tab<tab_plays>(tid);
		assert(tab != nullptr);
		actor_switch_process_plays pram;
		std::for_each(adata.mutable_m_member()->begin(), adata.mutable_m_member()->end(), [&pram](const pbnet::MATCHING_MEMBER& adata)
			{
				pram.m_players.push_back(adata.m_id());
			});

		i64_actorid lactorid = actor_guid::make(ACTOR_PLAYS_GO_UNDERGROUNDPALACE, ttab_servers::tab()->m_area, adata.m_roomid());
		switch (tab->m_type)
		{
		case pbnet::eplays_go_undergroundpalace:
			actor_create::switch_process(lactorid, -1, nconfig::m_nodeid, pram);
			break;
		}

		//actor_base::create(ACTOR_PLAYS, adata.m_roomid(), &pram);
		// ����ƥ�䷿����Խ�ɢ��
		//auto pro = std::shared_ptr<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>();
		//pro->set_m_type(adata.m_type());
		//pro->set_m_roomid(adata.m_roomid());
		//send_actor(actor_guid::make_self(ACTOR_MATCHING), pro);
		return true;
	}

	bool actor_manage_plays::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		return true;
	}
}