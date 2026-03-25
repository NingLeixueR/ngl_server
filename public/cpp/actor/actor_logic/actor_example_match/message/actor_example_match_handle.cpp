/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements message handlers for message.
#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_example_match/actor_example_match.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
namespace ngl
{
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>>& adata)
	{
		const auto* lparm = adata.get_data();
		i64_actorid lroleid = lparm->identifier();
		int32_t* lproomid = tools::findmap(m_matching, lroleid);
		if (lproomid == nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTFINDROOM, nullptr, 0, lroleid);
			return true;
		}

		room* lproom = find_room(*lproomid);
		if (lproom == nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTFINDROOM, nullptr, 0, lroleid);
			return true;
		}
		if (tools::findmap(lproom->m_players, lroleid) == nullptr)
		{
			m_matching.erase(lroleid);
			send_error(pbexample::EERROR_CODE_NOTMATCH, nullptr, *lproomid, lroleid);
			return true;
		}

		bool lsync = lproom->m_players.size() > 1;
		erase_player_room(lproom, lroleid);
		if (lsync)
		{
			sync_match_info(lproom);
		}
		return true;
	}
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>>& adata)
	{
		const auto* lparm = adata.get_data();
		i64_actorid lroleid = lparm->identifier();
		if (tools::findmap(m_matching, lroleid) != nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTMATCH, nullptr, 0, lroleid);
			return true;
		}
		const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN* lrecv = lparm->data();
		pbexample::EPLAY_TYPE ltype = lrecv->mtype();
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
		lproom->m_playersset.emplace(lroleid);
		m_matching.emplace(lroleid, lproom->m_roomid);
		if (room_count_ready(lproom))
		{
			auto& lroomidx = m_roomindex[lproom->m_type];
			lproom->m_roomready = localtime::gettime();
			lroomidx.m_readyroomlist.push_back(lproom->m_roomid);
			lroomidx.m_roomlist[lproom->m_roomid] = room_index::eroom_ready;
		}

		pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE pro;
		pro.set_mroomid(lproom->m_roomid);
		send_client(lroleid, pro);

		sync_match_info(lproom);
		return true;
	}
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		auto ldata = adata.get_data()->data();
		room* lproom = find_room(ldata->mroomid());
		if (lproom == nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTFINDROOM, nullptr, 0, lroleid);
			return true;
		}
		player* lpplayer = tools::findmap(lproom->m_players, lroleid);
		if (lpplayer == nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTMATCH, nullptr, ldata->mroomid(), lroleid);
			return true;
		}

		if (ldata->misconfirm())
		{
			lpplayer->m_isconfirm = true;
			sync_match_info(lproom);
			if (check_ready(lproom))
			{
				matching_finish(lproom);
			}
		}
		else
		{
			bool lsync = lproom->m_players.size() > 1;
			erase_player_room(lproom, lroleid);
			if (lsync)
			{
				sync_match_info(lproom);
			}
		}
		return true;
	}
	bool actor_example_match::handle(const message<np_login_request_info>& adata)
	{
		const auto* lrecv = adata.get_data();
		i64_actorid lroleid = lrecv->m_roleid;
		int32_t* lproomid = tools::findmap(m_matching, lroleid);
		if (lproomid == nullptr)
		{
			return true;
		}
		room* lproom = find_room(*lproomid);
		if (lproom == nullptr)
		{
			m_matching.erase(lroleid);
			return true;
		}
		sync_match_info(lproom, lrecv->m_roleid);
		return true;
	}
}//namespace ngl
