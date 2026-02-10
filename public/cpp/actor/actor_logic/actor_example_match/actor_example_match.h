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
#pragma once

#include "ttab_specialid.h"
#include "nmysql_manage.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "nmysql_pool.h"
#include "ndb_modular.h"
#include "example.pb.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_data.h"
#include "net.pb.h"
#include "ntimer.h"
#include "nmysql.h"
#include "ncsv.h"
#include "ntcp.h"

namespace ngl
{
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
		int32_t			m_index = 0;
	};

	struct room
	{
		pbexample::EPLAY_TYPE			m_type = pbexample::EPLAY_TYPE::EPLAY_NULL;
		int32_t							m_roomid = 0;
		int32_t							m_totalnumber = 0;		// 需要的总人数
		std::map<i64_actorid, player>	m_players;				// 参与匹配的玩家
		std::set<i64_actorid>			m_playersset;			// 参与匹配的玩家
		time_t							m_roomcreate = 0;		// 房间创建时间
		time_t							m_roomready = 0;		// 房间就绪时间
		int32_t							m_index = 0;			// 进入房间的序号
	};

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

	class actor_example_match : public actor
	{
		actor_example_match(const actor_example_match&) = delete;
		actor_example_match& operator=(const actor_example_match&) = delete;

		std::map<pbexample::EPLAY_TYPE, room_index>						m_roomindex;			// 房间号自增
		/////////roomid////////
		std::map<pbexample::EPLAY_TYPE, std::map<int32_t, room>>		m_room;
		std::map<i64_actorid, int32_t>									m_matching;				// key:roleid value:room id

		actor_example_match();
	public:
		friend class actor_instance<actor_example_match>;
		static actor_example_match& instance()
		{
			return actor_instance<actor_example_match>::instance();
		}

		virtual ~actor_example_match() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// # 同步房间信息
		void sync_match_info(room* aroom, i64_actorid aroleid = nguid::make());

		// # 同步[PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT]
		static void send_error(pbexample::PLAY_MATCHING_EERROR_CODE acode, std::set<i64_actorid>* aplayer = nullptr, int32_t aroomid = 0, i64_actorid aroleid = nguid::make());

		// # 房间是否人满就绪
		bool room_count_ready(room* aroom);

		// # 根据玩家离线数据选择匹配房间
		room* matching_room(i64_actorid aroleid, pbexample::EPLAY_TYPE atype);

		// # 查找房间
		room* find_room(int32_t aroomid);

		// # 查找房间
		room* find_room(pbexample::EPLAY_TYPE atype, int32_t aroomid);

		// # 增加房间
		room* add_room(pbexample::EPLAY_TYPE atype);

		// # 删除房间
		void erase_room(room* aroom, pbexample::PLAY_MATCHING_EERROR_CODE aerrorcode = pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_ROOM_DESTORY);

		// # 玩家取消匹配
		void erase_player_room(room* aroom, i64_actorid aroleid);

		// # 检查是否超时
		bool check_timeout(time_t atime, int32_t ainterval);

		// # 匹配成功
		void matching_finish(room* aroom);

		// # 检查就绪
		bool check_ready(room* aroom);

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>>& adata);
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>>& adata);
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata);
		bool handle(const message<np_login_request_info>& adata);
	};
}//namespace ngl
