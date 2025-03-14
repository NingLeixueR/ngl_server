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

			register_handle_custom<actor_example_match>::func<
				np_request_match_info
			>(true);
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
			time_t							m_roomready;		// 房间就绪时间
		};

		std::map<i64_actorid, int32_t>				m_matching;				// key:roleid value:room id
		struct room_index
		{
			int32_t				m_index = 0;
			enum eroom_stat
			{
				eroom_matching,		// 人未满匹配中
				eroom_ready,		// 人满就绪
			};
			std::map<int32_t, eroom_stat>	m_roomlist;
			std::list<int32_t>				m_readyroomlist;	// 人满就绪列表(顺序列表)
		};
		std::map<pbexample::EPLAY_TYPE, room_index>	m_roomindex;			// 房间号自增
										/////////roomid////////
		std::map<pbexample::EPLAY_TYPE, std::map<int32_t, room>>		m_room;

		// # 同步房间信息
		void sync_match_info(room* aroom, i64_actorid aroleid = nguid::make());

		// # 同步[PROBUFF_NET_EXAMPLE_PLAY_MATCHING_RESULT]
		void sync_response(room* aroom, pbexample::PLAY_EERROR_CODE acode, i64_actorid aroleid = nguid::make());

		// # 房间是否人满就绪
		bool room_ready(room* aroom);

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
					if (room_ready(&lpair.second) == false)
					{
						return &lpair.second;
					}
				}
				return add_room(atype);
			}
			return nullptr;
		}

		// 加入匹配
		bool handle(const message<mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN>>& adata)
		{
			i64_actorid lroleid = adata.get_data()->identifier();
			if (m_matching.contains(lroleid))
			{
				return true;
			}
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
			m_matching[lroleid] = lproom->m_roomid;
			if (room_ready(lproom))
			{
				lproom->m_roomready = localtime::gettime();
				m_roomindex[lproom->m_type].m_readyroomlist.push_back(lproom->m_roomid);
				m_roomindex[lproom->m_type].m_roomlist[lproom->m_roomid] = room_index::eroom_ready;
			}

			auto pro = std::make_shared<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN_RESPONSE>();
			pro->set_m_roomid(lproom->m_roomid);
			send_client(lroleid, pro);

			sync_match_info(lproom);
			return true;
		}

		room* find_room(int32_t aroomid)
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

		room* find_room(pbexample::EPLAY_TYPE atype, int32_t aroomid)
		{
			std::map<int32_t, room>* lmap = tools::findmap(m_room, atype);
			if (lmap != nullptr)
			{
				return tools::findmap(*lmap, aroomid);
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
			lroomindex.m_roomlist.insert(std::make_pair(lroomid, room_index::eroom_matching));
			room& lroom = m_room[atype][lroomid];
			lroom.m_type = atype;
			lroom.m_roomcreate = localtime::gettime();
			lroom.m_roomid = lroomid;
			lroom.m_totalnumber = ltotalnumber;
			return &lroom;
		}

		void erase_room(room* aroom, pbexample::PLAY_EERROR_CODE aerrorcode = pbexample::PLAY_EERROR_CODE::EERROR_CODE_ROOM_DESTORY)
		{
			sync_response(aroom, aerrorcode);
			room_index& lroomindex = m_roomindex[aroom->m_type];
			lroomindex.m_roomlist.erase(aroom->m_roomid);
			m_room[aroom->m_type].erase(aroom->m_roomid);
		}

		// # 确认准备好进入例子游戏
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
				// 退出就绪
				if (room_ready(lproom) == false)
				{
					lproom->m_roomready = 0;
					m_roomindex[lproom->m_type].m_roomlist[lproom->m_roomid] = room_index::eroom_matching;
					std::list<int32_t>& lready = m_roomindex[lproom->m_type].m_readyroomlist;
					for(auto itor = lready.begin();itor != lready.end();++itor)
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

		// # 玩家上线后请求匹配信息
		bool handle(const message<np_request_match_info>& adata)
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

		bool check_timeout(time_t atime, int32_t ainterval)
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

		void matching_finish(room* aroom)
		{
			//### 通知玩法管理器创建对应玩法actor



			erase_room(aroom, pbexample::PLAY_EERROR_CODE::EERROR_CODE_FINISH);
		}

		bool timer_handle(const message<timerparm>& adata)
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
	};
}//namespace ngl