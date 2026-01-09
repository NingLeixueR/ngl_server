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
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_example_match::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);

		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_example_match>::instance_readall(this, pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase"));
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
		// 定时器
		actor::register_timer<actor_example_match>(&actor_example_match::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_example_match>::func<
			np_login_request_info
			, mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>
			, mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>
			, mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>
		>(e_ready_all);
	}

	void actor_example_match::sync_match_info(room* aroom, i64_actorid aroleid /*= nguid::make()*/)
	{
		pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING pro;
		pro.set_mroomid(aroom->m_roomid);
		pro.set_mtotalnumber(aroom->m_totalnumber);
		pro.set_mroomcreate(aroom->m_roomcreate);
		pro.set_mroomready(aroom->m_roomready);
		for (const auto& [id, info] : aroom->m_players)
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
		const pbdb::db_brief* lpbrief = tdb_brief::nsp_cread<actor_example_match>::instance(id_guid()).getconst(aroleid);
		if (lpbrief == nullptr)
		{
			return nullptr;
		}
		std::map<int32_t, room>& lmap = m_room[atype];
		if ((atype & pbexample::EPLAY_TYPE::EPLAY_GUESS_NUMBER) != 0)
		{
			for (std::pair<const int32_t, room>& lpair : lmap)
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
		lroomindex.m_roomlist.insert(std::make_pair(lroomid, room_index::eroom_matching));
		room& lroom = m_room[atype][lroomid];
		lroom.m_type = atype;
		lroom.m_roomcreate = localtime::gettime();
		lroom.m_roomid = lroomid;
		lroom.m_totalnumber = ltotalnumber;

		log_error()->print("add room [{}]", lroomid);

		return &lroom;
	}

	void actor_example_match::erase_room(room* aroom, pbexample::PLAY_MATCHING_EERROR_CODE aerrorcode /*= pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_ROOM_DESTORY*/)
	{
		log_error()->print("erase room [{}]", aroom->m_roomid);
		for (i64_actorid roleid : aroom->m_playersset)
		{
			m_matching.erase(roleid);
		}
		send_error(aerrorcode, &aroom->m_playersset, aroom->m_roomid);
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
			erase_room(aroom, pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_CANCEL);
		}
		else
		{
			for (std::pair<const i64_actorid, player>& item : aroom->m_players)
			{
				item.second.m_isconfirm = false;
			}
		}
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
		auto pro = std::make_shared<np_create_example>();
		for (const std::pair<const i64_actorid, player>& lpair : aroom->m_players)
		{
			pro->m_roleids[lpair.second.m_index] = lpair.second.m_roleid;
		}
		pro->m_type = aroom->m_type;
		actor::send_actor(actor_example_manage::actorid(), id_guid(), pro);

		erase_room(aroom, pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_FINISH);
	}

	bool actor_example_match::check_ready(room* aroom)
	{
		for (const std::pair<const i64_actorid, player>& apair : aroom->m_players)
		{
			if (apair.second.m_isconfirm == false)
			{
				return false;
			}
		}
		return true;
	}

	bool actor_example_match::timer_handle(const message<np_timerparm>& adata)
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
				if (check_timeout(lproom->m_roomready, ttab_specialid::instance().m_example_room_readytime))
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
					if (check_timeout(lproom->m_roomcreate, ttab_specialid::instance().m_example_room_maxtime))
					{
						erase_room(lproom, pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_TIMEOUT);
					}
				}
			}
		}
		return true;
	}
}//namespace ngl