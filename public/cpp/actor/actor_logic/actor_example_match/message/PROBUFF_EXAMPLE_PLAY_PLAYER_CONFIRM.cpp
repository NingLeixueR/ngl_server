#include "actor_example_manage.h"
#include "actor_example_match.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		const pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM* ldata = adata.get_data()->data();
		room* lproom = find_room(ldata->m_roomid());
		if (lproom == nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTFINDROOM, nullptr, 0, lroleid);
			return true;
		}

		if (ldata->m_isconfirm())
		{
			player* lpplayer = tools::findmap(lproom->m_players, lroleid);
			lpplayer->m_isconfirm = true;
			sync_match_info(lproom);
			if (check_ready(lproom))
			{
				matching_finish(lproom);
			}
		}
		else
		{
			lproom->m_players.erase(lroleid);
			lproom->m_playersset.erase(lroleid);
			if (lproom->m_players.empty() || lproom->m_playersset.empty())
			{
				erase_room(lproom);
			}
			else
			{
				// ÍË³ö¾ÍÐ÷
				if (room_count_ready(lproom) == false)
				{
					lproom->m_roomready = 0;
					m_roomindex[lproom->m_type].m_roomlist[lproom->m_roomid] = room_index::eroom_matching;
					std::list<int32_t>& lready = m_roomindex[lproom->m_type].m_readyroomlist;
					for (auto itor = lready.begin(); itor != lready.end(); ++itor)
					{
						if (lproom->m_roomid == *itor)
						{
							lready.erase(itor);
							break;
						}
					}
				}
				sync_match_info(lproom);
			}
		}
		return true;
	}
}//namespace ngl