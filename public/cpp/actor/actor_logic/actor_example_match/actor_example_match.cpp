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
// File overview: Implements example match actor player pairing logic.


#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_example_match/actor_example_match.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"

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
	}

	ENUM_ACTOR actor_example_match::actor_type()
	{
		return ACTOR_EXAMPLE_MATCH;
	}

	i64_actorid actor_example_match::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_example_match::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_example_match::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);

		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_example_match>::instance_readall(this, tools::pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase"));
	}

	void actor_example_match::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_example_match>::instance(id_guid()).exit();
	}

	void actor_example_match::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_example_match::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_example_match::nregister()
	{
		// Timer.
		actor::register_timer<actor_example_match>(&actor_example_match::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_example_match
			, np_login_request_info
			, mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>
			, mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>
			, mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>
		>();
	}

	void actor_example_match::sync_match_info(room* aroom, i64_actorid aroleid /*= nguid::make()*/)
	{
		pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING pro;
		pro.set_mroomid(aroom->m_roomid);
		pro.set_mtotalnumber(aroom->m_totalnumber);
		pro.set_mroomcreate(aroom->m_roomcreate);
		pro.set_mroomready(aroom->m_roomready);
		for (auto& [id, info] : aroom->m_players)
		{
			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cread<actor_example_match>::instance(id_guid()).getconst(id);
			if (lpbrief != nullptr)
			{
				pbexample::PLAYER& lplayer = *pro.mutable_mplayers()->Add();
				lplayer.set_misconfirm(info.m_isconfirm);
				*lplayer.mutable_mplayers() = *lpbrief;
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

	void actor_example_match::send_error(pbexample::PLAY_MATCHING_EERROR_CODE acode, std::set<i64_actorid>* aplayer/* = nullptr*/, int32_t aroomid /*= 0*/, i64_actorid aroleid /*= nguid::make()*/)
	{
		pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT pro;
		pro.set_mroomid(aroomid);
		pro.set_merrorcode(acode);
		if (aplayer != nullptr)
		{
			send_client(*aplayer, pro);
		}
		else
		{
			if (aroleid != nguid::make())
			{
				send_client(aroleid, pro);
			}
		}
	}

	bool actor_example_match::room_count_ready(room* aroom)
	{
		if (aroom->m_playersset.size() >= aroom->m_totalnumber)
		{
			return true;
		}
		return false;
	}

	room* actor_example_match::matching_room(i64_actorid aroleid, pbexample::EPLAY_TYPE atype)
	{
		if (tdb_brief::nsp_cread<actor_example_match>::instance(id_guid()).getconst(aroleid) == nullptr)
		{
			return nullptr;
		}
		auto lmap_it = m_room.try_emplace(atype).first;
		std::map<int32_t, room>& lmap = lmap_it->second;
		if ((atype & pbexample::EPLAY_TYPE::EPLAY_GUESS_NUMBER) != 0)
		{
			for (auto& lpair : lmap)
			{
				if (room_count_ready(&lpair.second) == false)
				{
					return &lpair.second;
				}
			}
			return add_room(atype);
		}
		return nullptr;
	}

	room* actor_example_match::find_room(int32_t aroomid)
	{
		for (auto& [type, rindex] : m_roomindex)
		{
			if (tools::findmap(rindex.m_roomlist, aroomid) != nullptr)
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

	room* actor_example_match::find_room(pbexample::EPLAY_TYPE atype, int32_t aroomid)
	{
		std::map<int32_t, room>* lmap = tools::findmap(m_room, atype);
		if (lmap != nullptr)
		{
			return tools::findmap(*lmap, aroomid);
		}
		return nullptr;
	}

	room* actor_example_match::add_room(pbexample::EPLAY_TYPE atype)
	{
		int32_t ltotalnumber = ttab_specialid::instance().m_example_totalnumber[atype];
		if (ltotalnumber <= 0)
		{
			return nullptr;
		}

		room_index& lroomindex = m_roomindex[atype];
		int32_t lroomid = ++lroomindex.m_index;
		lroomindex.m_roomlist.emplace(lroomid, room_index::eroom_matching);
		room& lroom = m_room[atype][lroomid];
		lroom.m_type = atype;
		lroom.m_roomcreate = tools::time::gettime();
		lroom.m_roomid = lroomid;
		lroom.m_totalnumber = ltotalnumber;

		log_error()->print("add room [{}]", lroomid);

		return &lroom;
	}

	void actor_example_match::erase_ready(pbexample::EPLAY_TYPE atype, int32_t aroomid)
	{
		auto lidx_it = m_roomindex.find(atype);
		if (lidx_it == m_roomindex.end())
		{
			return;
		}

		std::list<int32_t>& lready = lidx_it->second.m_readyroomlist;
		for (auto litor = lready.begin(); litor != lready.end(); ++litor)
		{
			if (*litor == aroomid)
			{
				lready.erase(litor);
				break;
			}
		}
	}

	void actor_example_match::erase_room(room* aroom, pbexample::PLAY_MATCHING_EERROR_CODE aerrorcode /*= pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_ROOM_DESTORY*/)
	{
		log_error()->print("erase room [{}]", aroom->m_roomid);
		for (const i64_actorid lroleid : aroom->m_playersset)
		{
			m_matching.erase(lroleid);
		}
		send_error(aerrorcode, &aroom->m_playersset, aroom->m_roomid);
		room_index& lroomindex = m_roomindex[aroom->m_type];
		erase_ready(aroom->m_type, aroom->m_roomid);
		lroomindex.m_roomlist.erase(aroom->m_roomid);
		auto lroom_it = m_room.find(aroom->m_type);
		if (lroom_it != m_room.end())
		{
			lroom_it->second.erase(aroom->m_roomid);
		}
	}

	void actor_example_match::erase_player_room(room* aroom, i64_actorid aroleid)
	{
		bool lready = aroom->m_roomready != 0;
		m_matching.erase(aroleid);
		aroom->m_players.erase(aroleid);
		aroom->m_playersset.erase(aroleid);
		aroom->m_roomready = 0;
		if (aroom->m_players.empty())
		{
			erase_room(aroom, pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_CANCEL);
		}
		else
		{
			if (lready)
			{
				erase_ready(aroom->m_type, aroom->m_roomid);
			}
			room_index& lroomindex = m_roomindex[aroom->m_type];
			lroomindex.m_roomlist.insert_or_assign(aroom->m_roomid, room_index::eroom_matching);
			for (auto& lpair : aroom->m_players)
			{
				lpair.second.m_isconfirm = false;
			}
		}
	}

	bool actor_example_match::check_timeout(time_t atime, int32_t ainterval)
	{
		time_t lnow = tools::time::gettime();
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
		// Notify the game play manager to create the corresponding gameplay actor
		auto pro = std::make_shared<np_create_example>();
		for (const auto& lpair : aroom->m_players)
		{
			pro->m_roleids[lpair.second.m_index] = lpair.second.m_roleid;
		}
		pro->m_type = aroom->m_type;
		actor::send_actor(actor_example_manage::actorid(), id_guid(), pro);

		erase_room(aroom, pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_FINISH);
	}

	bool actor_example_match::check_ready(room* aroom)
	{
		for (const auto& lpair : aroom->m_players)
		{
			if (lpair.second.m_isconfirm == false)
			{
				return false;
			}
		}
		return true;
	}

	bool actor_example_match::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		// Process ready rooms
		for (auto& lpair : m_roomindex)
		{
			std::list<int32_t> lreadyids = lpair.second.m_readyroomlist;
			for (const int32_t lroomid : lreadyids)
			{
				room* lproom = find_room(lpair.first, lroomid);
				if (lproom == nullptr)
				{
					erase_ready(lpair.first, lroomid);
					continue;
				}
				if (check_timeout(lproom->m_roomready, ttab_specialid::instance().m_example_room_readytime))
				{
					matching_finish(lproom);
				}
				else
				{// Ready list is sorted by time, so if this room hasn't timed out, later ones won't either
					break;
				}
			}
		}

		for (const auto& lpair : m_roomindex)
		{
			std::list<int32_t> lroomids;
			for (const auto& litem : lpair.second.m_roomlist)
			{
				if (litem.second == room_index::eroom_matching)
				{
					lroomids.push_back(litem.first);
				}
			}
			for (const int32_t lroomid : lroomids)
			{
				room* lproom = find_room(lpair.first, lroomid);
				if (lproom == nullptr)
				{
					continue;
				}
				if (check_timeout(lproom->m_roomcreate, ttab_specialid::instance().m_example_room_maxtime))
				{
					erase_room(lproom, pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_TIMEOUT);
				}
			}
		}
		return true;
	}
}//namespace ngl
