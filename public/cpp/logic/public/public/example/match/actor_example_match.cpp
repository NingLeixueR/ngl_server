#include "actor_example_match.h"
#include "actor_brief.h"

namespace ngl
{
	actor_example_match::actor_example_match() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_EXAMPLE_MATCH,
						.m_area = tab_self_area
					},
				.m_weight = 0x7fffffff,
			})
	{
		std::set<i64_actorid> ldataid;
		tdb_brief::nsp_cli<actor_example_match>::init(actor_brief::actor_type(), this, ldataid);
	}

	void actor_example_match::sync_match_info(room* aroom)
	{
		auto pro = std::make_shared<pbexample::PROBUFF_NET_EXAMPLE_PLAY_MATCHING>();
		pro->set_m_roomid(aroom->m_roomid);
		pro->set_m_totalnumber(aroom->m_totalnumber);
		for (const auto [id, info] : aroom->m_players)
		{
			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::getconst(id);
			if (lpbrief != nullptr)
			{
				pbexample::PLAYER& lplayer = *pro->mutable_m_players()->Add();
				lplayer.set_m_isconfirm(info.m_isconfirm);
				*lplayer.mutable_m_players() = *lpbrief;
			}
		}
		send_client(aroom->m_playersset, pro);
	}

	void actor_example_match::sync_response(room* aroom, pbexample::PLAY_EERROR_CODE acode, i64_actorid aroleid /*= nguid::make()*/)
	{
		auto pro = std::make_shared<pbexample::PROBUFF_NET_EXAMPLE_PLAY_MATCHING_RESULT>();
		pro->set_m_roomid(aroom->m_roomid);
		pro->set_m_errorcode(acode);
		if (aroleid == nguid::make())
		{
			send_client(aroom->m_playersset, pro);
		}
		else
		{
			send_client(aroleid, pro);
		}
	}
}//namespace ngl