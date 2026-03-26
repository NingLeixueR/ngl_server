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
// File overview: Declares interfaces for actor example match.

#pragma once

#include "actor/actor_logic/actor_create/actor_create.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/tab/ttab_specialid.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "actor/generated/pb/example.pb.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/generated/pb/net.pb.h"
#include "net/tcp/ntcp.h"

#include <list>

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
			m_value[0] = static_cast<int16_t>(atype);
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
		bool			m_isconfirm = false; // Whetherconfirm
		int32_t			m_index = 0;
	};

	struct room
	{
		pbexample::EPLAY_TYPE			m_type = pbexample::EPLAY_TYPE::EPLAY_NULL;
		int32_t							m_roomid = 0;
		int32_t							m_totalnumber = 0;		// Need to
		std::map<i64_actorid, player>	m_players;				// And player
		std::set<i64_actorid>			m_playersset;			// And player
		time_t							m_roomcreate = 0;		// Roomcreatetime
		time_t							m_roomready = 0;		// Roomreadytime
		int32_t							m_index = 0;			// Room
	};

	struct room_index
	{
		int32_t				m_index = 0;
		enum eroom_stat
		{
			eroom_matching,		// In
			eroom_ready,		// Ready
		};
		std::map<int32_t, eroom_stat>	m_roomlist;
		std::list<int32_t>				m_readyroomlist;	// Readylist( list)
	};

	class actor_example_match : public actor
	{
		actor_example_match(const actor_example_match&) = delete;
		actor_example_match& operator=(const actor_example_match&) = delete;

		std::map<pbexample::EPLAY_TYPE, room_index>						m_roomindex;			// Room
		/////////roomid////////
		std::map<pbexample::EPLAY_TYPE, std::map<int32_t, room>>		m_room;
		std::map<i64_actorid, int32_t>									m_matching;				// key:roleid value:room id
		void erase_ready(pbexample::EPLAY_TYPE atype, int32_t aroomid);

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

		// # Synchronizeroominfo
		void sync_match_info(room* aroom, i64_actorid aroleid = nguid::make());

		// # Synchronize[PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT]
		static void send_error(pbexample::PLAY_MATCHING_EERROR_CODE acode, std::set<i64_actorid>* aplayer = nullptr, int32_t aroomid = 0, i64_actorid aroleid = nguid::make());

		// # Roomwhether ready
		bool room_count_ready(room* aroom);

		// # Player data room
		room* matching_room(i64_actorid aroleid, pbexample::EPLAY_TYPE atype);

		// # Findroom
		room* find_room(int32_t aroomid);

		// # Findroom
		room* find_room(pbexample::EPLAY_TYPE atype, int32_t aroomid);

		// # Room
		room* add_room(pbexample::EPLAY_TYPE atype);

		// # Deleteroom
		void erase_room(room* aroom, pbexample::PLAY_MATCHING_EERROR_CODE aerrorcode = pbexample::PLAY_MATCHING_EERROR_CODE::EERROR_CODE_ROOM_DESTORY);

		// # Playercancel
		void erase_player_room(room* aroom, i64_actorid aroleid);

		// # Whether
		bool check_timeout(time_t atime, int32_t ainterval);

		// # Successful
		void matching_finish(room* aroom);

		// # Ready
		bool check_ready(room* aroom);

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata);
		bool handle([[maybe_unused]] const message<np_login_request_info>& adata);
	};
}//namespace ngl
