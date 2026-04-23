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
// File overview: Robot actor that simulates a player client for automated testing and stress testing.

#pragma once

#include "actor/actor_base/core/actor_manage.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/generated/pb/example.pb.h"
#include "actor/generated/pb/net.pb.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "tools/db/sql/db_data.h"
#include "tools/ai/nbt.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	struct thruput
	{
		std::atomic<int32_t> m_count = 0;
		int32_t m_maxcount = 0;
		time_t m_beg = 0;
		time_t m_end = 0;
	};
	class test_thruput
	{
		std::vector<std::shared_ptr<thruput>> m_rounds;
	public:
		static test_thruput& instance()
		{
			static test_thruput ltemp;
			return ltemp;
		}

		void change_rounds(int arounds)
		{
			m_rounds[arounds]->m_count.fetch_add(1, std::memory_order_relaxed);
			if (m_rounds[arounds]->m_count >= m_rounds[arounds]->m_maxcount)
			{
				m_rounds[arounds]->m_end = ngl::tools::time_wheel::getms();
				test_thruput::instance().print(arounds);
			}
		}

		void add_rounds(int32_t aactorcount, int32_t aeverycount)
		{
			auto lpthruput = std::make_shared<thruput>();
			lpthruput->m_count = 0;
			lpthruput->m_maxcount = aactorcount * aeverycount;
			lpthruput->m_beg = ngl::tools::time_wheel::getms();
			lpthruput->m_end = 0;
			m_rounds.emplace_back(lpthruput);

			auto pro = std::make_shared<np_thruput_test>();
			pro->m_rounds = (int32_t)m_rounds.size() - 1;
			for (int i = 0; i < aeverycount; ++i)
			{
				actor::send_actor(ACTOR_ROBOT, pro);
			}
		}

		void print(int arounds)
		{
			std::cout << std::format("rounds:{} m_count:{} m_maxcount:{} beg:{} end:{} space:{}\n", 
				arounds, m_rounds[arounds]->m_count.load(), m_rounds[arounds]->m_maxcount, 
				m_rounds[arounds]->m_beg, m_rounds[arounds]->m_end, m_rounds[arounds]->m_end - m_rounds[arounds]->m_beg
			);
		}

		void release()
		{
			m_rounds.clear();
		}
	};

	struct _robot;
	
	class actor_robot : 
		public actor
	{
		// ----- Data Begin -----
		pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE	m_data;
		static nbt_factory<actor_robot>			m_factory;
		nbt_context<actor_robot>				m_bt;
		nbt_status								m_status = BT::NodeStatus::FAILURE;
		// ----- Data End   -----
		nbt_status is_kcp_connected(pbnet::ENUM_KCP akcpenum);
		nbt_status ensure_kcp_connected(pbnet::ENUM_KCP akcpenum);
	public:
		i32_session								m_session = 0;
		_robot*									m_robot = nullptr;
		bool									m_firstsync = false;
	public:
		actor_robot() = delete;
		actor_robot(const actor_robot&) = delete;
		actor_robot& operator=(const actor_robot&) = delete;

		actor_robot(i16_area aarea, i32_actordataid arobotid, void*);

		virtual ~actor_robot() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(int32_t adataid);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		void ukcp_connect(pbnet::ENUM_KCP akcpenum);

		bool handle([[maybe_unused]] const message<np_thruput_test>& adata);
		bool handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>& adata);
		bool handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata);
		bool handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata);
		bool handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata);
		bool handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata);
		bool handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ERROR>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLESTAT>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SYNC_POSITION>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SYNC_UNIT>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>& adata);
	};

	struct _robot
	{
		i32_sessionid					m_session = -1;
		ENET_PROTOCOL					m_protocol = ENET_TCP;
		std::string						m_account;
		std::shared_ptr<actor_robot>	m_robot = nullptr;
		i64_actorid						m_actor_roleid = nguid::make();
		i32_serverid					m_gameid = -1;
		i32_serverid					m_gatewayid = -1;
	};
}//namespace ngl
