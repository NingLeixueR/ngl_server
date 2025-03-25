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

	void actor_example_match::sync_match_info(room* aroom, i64_actorid aroleid /*= nguid::make()*/)
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>();
		pro->set_m_roomid(aroom->m_roomid);
		pro->set_m_totalnumber(aroom->m_totalnumber);
		pro->set_m_roomcreate(aroom->m_roomcreate);
		pro->set_m_roomready(aroom->m_roomready);
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
		if (aroleid == nguid::make())
		{
			send_client(aroom->m_playersset, pro);
		}
		else
		{
			send_client(aroleid, pro);
		}
	}

	void actor_example_match::sync_response(room* aroom, pbexample::PLAY_EERROR_CODE acode, i64_actorid aroleid /*= nguid::make()*/)
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>();
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

	bool actor_example_match::room_ready(room* aroom)
	{
		if (aroom->m_totalnumber >= aroom->m_playersset.size())
		{
			return true;
		}
		return false;
	}

	actor_example_match::room* actor_example_match::matching_room(i64_actorid aroleid, pbexample::EPLAY_TYPE atype)
	{
		const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::getconst(aroleid);
		if (lpbrief == nullptr)
		{
			return nullptr;
		}
		std::map<int32_t, room>& lmap = m_room[atype];
		if (atype == pbexample::EPLAY_TYPE::EPLAY_GUESS_NUMBER)
		{
			for (std::pair<const int32_t, room>& lpair : lmap)
			{
				if (room_ready(&lpair.second) == false)
				{
					return &lpair.second;
				}
			}
			return add_room(atype);
		}
		return nullptr;
	}

	actor_example_match::room* actor_example_match::find_room(int32_t aroomid)
	{
		for (const auto& [type, rindex] : m_roomindex)
		{
			if (rindex.m_roomlist.contains(aroomid))
			{
				std::map<int32_t, room>* lmap = tools::findmap(m_room, type);
				if (lmap != nullptr)
				{
					return tools::findmap(*lmap, aroomid);
				}
			}
		}
		return nullptr;
	}

	actor_example_match::room* actor_example_match::find_room(pbexample::EPLAY_TYPE atype, int32_t aroomid)
	{
		std::map<int32_t, room>* lmap = tools::findmap(m_room, atype);
		if (lmap != nullptr)
		{
			return tools::findmap(*lmap, aroomid);
		}
		return nullptr;
	}

	actor_example_match::room* actor_example_match::add_room(pbexample::EPLAY_TYPE atype)
	{
		int32_t ltotalnumber = ttab_specialid::m_example_totalnumber[atype];
		if (ltotalnumber <= 0)
		{
			return nullptr;
		}

		room_index& lroomindex = m_roomindex[atype];
		int32_t lroomid = ++lroomindex.m_index;
		lroomindex.m_roomlist.insert(std::make_pair(lroomid, room_index::eroom_matching));
		room& lroom = m_room[atype][lroomid];
		lroom.m_type = atype;
		lroom.m_roomcreate = localtime::gettime();
		lroom.m_roomid = lroomid;
		lroom.m_totalnumber = ltotalnumber;
		return &lroom;
	}

	void actor_example_match::erase_room(room* aroom, pbexample::PLAY_EERROR_CODE aerrorcode /*= pbexample::PLAY_EERROR_CODE::EERROR_CODE_ROOM_DESTORY*/)
	{
		sync_response(aroom, aerrorcode);
		room_index& lroomindex = m_roomindex[aroom->m_type];
		lroomindex.m_roomlist.erase(aroom->m_roomid);
		m_room[aroom->m_type].erase(aroom->m_roomid);
	}

	void actor_example_match::erase_player_room(room* aroom, i64_actorid aroleid)
	{
		aroom->m_players.erase(aroleid);
		aroom->m_playersset.erase(aroleid);
		aroom->m_roomready = 0;
		if (aroom->m_players.empty())
		{
			erase_room(aroom, pbexample::PLAY_EERROR_CODE::EERROR_CODE_CANCEL);
		}
		else
		{
			for (std::pair<const i64_actorid, player>& item : aroom->m_players)
			{
				item.second.m_isconfirm = false;
			}
		}
	}

	void actor_example_match::init()
	{
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	bool actor_example_match::check_timeout(time_t atime, int32_t ainterval)
	{
		time_t lnow = localtime::gettime();
		if (lnow < atime)
		{
			return false;
		}
		time_t ltemp = lnow - atime;
		if (ltemp >= ainterval)
		{
			return true;
		}
		return false;
	}

	void actor_example_match::matching_finish(room* aroom)
	{
		//### 通知玩法管理器创建对应玩法actor



		erase_room(aroom, pbexample::PLAY_EERROR_CODE::EERROR_CODE_FINISH);
	}

	bool actor_example_match::timer_handle(const message<timerparm>& adata)
	{
		// 检查就绪
		for (std::pair<const pbexample::EPLAY_TYPE, room_index>& item : m_roomindex)
		{
			for (int32_t roomid : item.second.m_readyroomlist)
			{
				room* lproom = find_room(item.first, roomid);
				if (lproom == nullptr)
				{
					continue;
				}
				if (check_timeout(lproom->m_roomready, ttab_specialid::example_room_readytime))
				{
					matching_finish(lproom);
				}
				else
				{// m_readyroomlist 是按照就绪顺序排列的链表，如果出现一个没到时间的说明后面也不到时间
					break;
				}
			}
		}

		for (std::pair<const pbexample::EPLAY_TYPE, room_index>& item : m_roomindex)
		{
			for (auto [roomid, type] : item.second.m_roomlist)
			{
				if (type == room_index::eroom_matching)
				{
					room* lproom = find_room(item.first, roomid);
					if (lproom == nullptr)
					{
						continue;
					}
					if (check_timeout(lproom->m_roomcreate, ttab_specialid::m_example_room_maxtime))
					{
						erase_room(lproom, pbexample::PLAY_EERROR_CODE::EERROR_CODE_TIMEOUT);
					}
				}
			}
		}
		ttab_specialid::m_example_room_maxtime;
		return true;
	}

	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		if (m_matching.contains(lroleid))
		{
			return true;
		}
		const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN* ldata = adata.get_data()->data();
		pbexample::EPLAY_TYPE ltype = ldata->m_type();
		room* lproom = matching_room(lroleid, ltype);
		if (lproom == nullptr)
		{
			return true;
		}
		player& lplayer = lproom->m_players[lroleid];
		lplayer.m_isconfirm = false;
		lplayer.m_roleid = lroleid;
		lproom->m_playersset.insert(lroleid);
		m_matching[lroleid] = lproom->m_roomid;
		if (room_ready(lproom))
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

	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		const pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM* ldata = adata.get_data()->data();
		room* lproom = find_room(ldata->m_roomid());
		if (lproom == nullptr)
		{
			sync_response(nullptr, pbexample::PLAY_EERROR_CODE::EERROR_CODE_NOTFINDROOM);
			return true;
		}

		if (ldata->m_isconfirm())
		{
			player* lpplayer = tools::findmap(lproom->m_players, lroleid);
			lpplayer->m_isconfirm = true;
		}
		else
		{
			lproom->m_players.erase(lroleid);
			lproom->m_playersset.erase(lroleid);
			if (lproom->m_players.empty() || lproom->m_playersset.empty())
			{
				erase_room(lproom);
			}
			// 退出就绪
			if (room_ready(lproom) == false)
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
		}

		sync_match_info(lproom);
		return true;
	}

	bool actor_example_match::handle(const message<np_request_match_info>& adata)
	{
		int32_t* lproomid = tools::findmap(m_matching, adata.get_data()->m_roleid);
		if (lproomid == nullptr)
		{
			return true;
		}
		room* lproom = find_room(*lproomid);
		if (lproom == nullptr)
		{
			return true;
		}
		sync_match_info(lproom, adata.get_data()->m_roleid);
		return true;
	}

	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		int32_t* lproomid = tools::findmap(m_matching, lroleid);
		if (lproomid == nullptr)
		{
			sync_response(nullptr, pbexample::PLAY_EERROR_CODE::EERROR_CODE_NOTFINDROOM, lroleid);
			return true;
		}

		room* lproom = find_room(*lproomid);
		if (lproom == nullptr)
		{
			sync_response(nullptr, pbexample::PLAY_EERROR_CODE::EERROR_CODE_NOTFINDROOM, lroleid);
			return true;
		}

		erase_player_room(lproom, lroleid);
		return true;
	}
}//namespace ngl