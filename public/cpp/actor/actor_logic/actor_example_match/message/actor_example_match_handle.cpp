/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_example_manage.h"
#include "actor_example_match.h"
#include "actor_brief.h"
namespace ngl
{
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
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

		erase_player_room(lproom, lroleid);
		return true;
	}
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		if (m_matching.contains(lroleid))
		{
			send_error(pbexample::EERROR_CODE_NOTMATCH, nullptr, 0, lroleid);
			return true;
		}
		const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN* ldata = adata.get_data()->data();
		pbexample::EPLAY_TYPE ltype = ldata->mtype();
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

		pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE pro;
		pro.set_mroomid(lproom->m_roomid);
		send_client(lroleid, pro);

		sync_match_info(lproom);
		return true;
	}
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		const pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM* ldata = adata.get_data()->data();
		room* lproom = find_room(ldata->mroomid());
		if (lproom == nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTFINDROOM, nullptr, 0, lroleid);
			return true;
		}

		if (ldata->misconfirm())
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
				// 退出就绪
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
	bool actor_example_match::handle(const message<np_login_request_info>& adata)
	{
		i64_actorid lroleid = adata.get_data()->m_roleid;
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
		sync_match_info(lproom, adata.get_data()->m_roleid);
		return true;
	}
}//namespace ngl
