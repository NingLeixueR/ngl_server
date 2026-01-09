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

#include "actor_manage.h"
#include "example.pb.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

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
		std::vector<thruput*> m_rounds;
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
				m_rounds[arounds]->m_end = ngl::time_wheel::getms();
				test_thruput::instance().print(arounds);
			}
		}

		void add_rounds(int32_t aactorcount, int32_t aeverycount)
		{
			m_rounds.push_back(new thruput
			{
				.m_count = 0,
				.m_maxcount = aactorcount * aeverycount,
				.m_beg = ngl::time_wheel::getms(),
				.m_end = 0
			});

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
			for (int32_t i = 0; i < m_rounds.size(); ++i)
			{
				delete m_rounds[i];
			}
			m_rounds.clear();
		}
	};

	struct _robot;
	
	class actor_robot : 
		public actor
	{
		// ----- Data Begin -----
		pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE	m_data;
	public:
		i32_session								m_session = 0;
		_robot*									m_robot = nullptr;
		bool									m_firstsync = false;
		// ----- Data End   -----
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

		bool timer_handle(const message<np_timerparm>& adata);

		void ukcp_connect(pbnet::ENUM_KCP akcpenum);

		bool handle(const message<np_thruput_test>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ERROR>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ROLESTAT>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_POSITION>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_UNIT>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>& adata);
	};

	struct _robot
	{
		i32_sessionid					m_session = -1;
		std::string						m_account;
		std::shared_ptr<actor_robot>	m_robot = nullptr;
		i64_actorid						m_actor_roleid = nguid::make();
		i32_serverid					m_gameid = -1;
		i32_serverid					m_gatewayid = -1;
	};
}//namespace ngl
