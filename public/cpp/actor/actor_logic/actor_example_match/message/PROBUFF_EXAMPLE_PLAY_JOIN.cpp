#include "actor_example_manage.h"
#include "actor_example_match.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		if (m_matching.contains(lroleid))
		{
			send_error(pbexample::EERROR_CODE_NOTMATCH, nullptr, 0, lroleid);
			return true;
		}
		const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN* ldata = adata.get_data()->data();
		pbexample::EPLAY_TYPE ltype = ldata->m_type();
		room* lproom = matching_room(lroleid, ltype);
		if (lproom == nullptr)
		{
			send_error(pbexample::EERROR_CODE_CREATEROOM_FAIL, nullptr, 0, lroleid);
			return true;
		}
		player& lplayer = lproom->m_players[lroleid];
		lplayer.m_isconfirm = false;
		lplayer.m_roleid = lroleid;
		lplayer.m_index = ++lproom->m_index;
		lproom->m_playersset.insert(lroleid);
		m_matching[lroleid] = lproom->m_roomid;
		if (room_count_ready(lproom))
		{
			lproom->m_roomready = localtime::gettime();
			m_roomindex[lproom->m_type].m_readyroomlist.push_back(lproom->m_roomid);
			m_roomindex[lproom->m_type].m_roomlist[lproom->m_roomid] = room_index::eroom_ready;
		}

		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>();
		pro->set_m_roomid(lproom->m_roomid);
		send_client(lroleid, pro);

		sync_match_info(lproom);
		return true;
	}
}//namespace ngl