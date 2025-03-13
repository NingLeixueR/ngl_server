#pragma once

#include "ttab_specialid.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	class actor_example_match : public actor
	{
		actor_example_match(const actor_example_match&) = delete;
		actor_example_match& operator=(const actor_example_match&) = delete;

		actor_example_match();
	public:
		friend class actor_instance<actor_example_match>;
		static actor_example_match& getInstance()
		{
			return actor_instance<actor_example_match>::instance();
		}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_EXAMPLE_MATCH;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_EXAMPLE_MATCH, tab_self_area, nguid::none_actordataid());
		}

		virtual ~actor_example_match() {}

		static void nregister()
		{
			// 定时器
			actor::register_timer<actor_example_match>(&actor_example_match::timer_handle);
			// 协议注册
			register_handle_proto<actor_example_match>::func<
				mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN>,
				mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_PLAYER_CONFIRM>
			>(false);
		}

		virtual void loaddb_finish(bool adbishave) {}

		union nroomid
		{
		private:
			int32_t m_id;
			int16_t m_value[2];// m_value2[0] = type		m_value2[1] = room index
		public:
			nroomid(int32_t aid) :
				m_id(aid)
			{}

			nroomid(pbexample::EPLAY_TYPE atype, int16_t aindex)
			{
				m_value[0] = atype;
				m_value[1] = aindex;
			}

			int32_t roomid()
			{
				return m_id;
			}

			pbexample::EPLAY_TYPE type()
			{
				return (pbexample::EPLAY_TYPE)m_value[0];
			}

			int16_t index()
			{
				return m_value[1];
			}

			operator int32_t ()const
			{
				return m_id;
			}
		};

		struct player
		{
			i64_actorid		m_roleid = 0;
			bool			m_isconfirm = false; // 是否确认
		};

		struct room
		{
			pbexample::EPLAY_TYPE			m_type;
			int32_t							m_roomid;
			int32_t							m_totalnumber;		// 需要的总人数
			std::map<i64_actorid, player>	m_players;			// 参与匹配的玩家
			std::set<i64_actorid>			m_playersset;		// 参与匹配的玩家
			time_t							m_roomcreate;		// 房间创建时间
		};

		std::map<i64_actorid, int32_t>				m_matching;				// key:roleid value:room id
		struct room_index
		{
			int32_t				m_index = 0;
			std::set<int32_t>	m_roomlist;
		};
		std::map<pbexample::EPLAY_TYPE, room_index>	m_roomindex;			// 房间号自增
										/////////roomid////////
		std::map<pbexample::EPLAY_TYPE, std::map<int32_t, room>>		m_room;

		void sync_match_info(room* aroom);

		void sync_response(room* aroom, pbexample::PLAY_EERROR_CODE acode, i64_actorid aroleid = nguid::make());

		// 根据玩家离线数据选择匹配房间
		room* matching_room(i64_actorid aroleid, pbexample::EPLAY_TYPE atype)
		{
			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_friends>::getconst(aroleid);
			if (lpbrief == nullptr)
			{
				return nullptr;
			}
			std::map<int32_t, room>* lmap = tools::findmap(m_room, atype);
			if (atype == pbexample::EPLAY_TYPE::EPLAY_GUESS_NUMBER)
			{
				for (std::pair<const int32_t, room>& lpair : *lmap)
				{
					if (lpair.second.m_totalnumber < lpair.second.m_playersset.size())
					{
						return &lpair.second;
					}
				}
				return add_room(atype);
			}
			return nullptr;
		}


		bool handle(const message<mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN>>& adata)
		{
			i64_actorid lroleid = adata.get_data()->identifier();
			const pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN* ldata = adata.get_data()->data();
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

			auto pro = std::make_shared<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN_RESPONSE>();
			pro->set_m_roomid(lproom->m_roomid);
			pro->set_m_waittime(100);
			send_client(lroleid, pro);

			sync_match_info(lproom);
			return true;
		}

		room* find_room(int32_t aroomid)
		{
			//std::map<pbexample::EPLAY_TYPE, room_index>	m_roomindex;
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

		room* add_room(pbexample::EPLAY_TYPE atype)
		{
			int32_t ltotalnumber = ttab_specialid::m_example_totalnumber[atype];
			if (ltotalnumber <= 0)
			{
				return nullptr;
			}

			room_index& lroomindex = m_roomindex[atype];
			int32_t lroomid = ++lroomindex.m_index;
			lroomindex.m_roomlist.insert(lroomid);
			room& lroom = m_room[atype][lroomid];
			lroom.m_type = atype;
			lroom.m_roomcreate = localtime::gettime();
			lroom.m_roomid = lroomid;
			lroom.m_totalnumber = ltotalnumber;
			return &lroom;
		}

		void erase_room(room* aroom)
		{
			sync_response(aroom, pbexample::PLAY_EERROR_CODE::EERROR_CODE_ROOM_DESTORY);
			room_index& lroomindex = m_roomindex[aroom->m_type];
			lroomindex.m_roomlist.erase(aroom->m_roomid);
			m_room[aroom->m_type].erase(aroom->m_roomid);
		}

		bool handle(const message<mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata)
		{
			i64_actorid lroleid = adata.get_data()->identifier();
			const pbexample::PROBUFF_NET_EXAMPLE_PLAY_PLAYER_CONFIRM* ldata = adata.get_data()->data();
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
			}

			sync_match_info(lproom);
			return true;
		}

		virtual void init()
		{
			timerparm tparm;
			if (make_timerparm::make_interval(tparm, 1) == false)
			{
				log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
				return;
			}
			set_timer(tparm);
		}

		bool timer_handle(const message<timerparm>& adata)
		{
			ttab_specialid::m_example_room_maxtime;
			return true;
		}
	};
}//namespace ngl