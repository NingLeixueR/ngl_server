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
// File overview: Implements logic for actor robot.


#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "actor/actor_logic/actor_robot/actor_robot.h"
#include "actor/actor_base/core/nregister.h"
#include "actor/actor_logic/nforward.h"
#include "net/udp/kcp/nkcp.h"

namespace ngl
{
	namespace
	{
		constexpr int32_t erobot_bt_tick_interval_sec = 1;
		constexpr int64_t erobot_kcp_retry_interval_ms = 3 * tools::time::MILLISECOND;

		const char* robot_behavior_tree_xml()
		{
			return R"(
<root BTCPP_format="4">
    <BehaviorTree ID="RobotBootstrap">
        <ReactiveSequence>
            <HasSession/>
            <HasRoleSync/>
            <EnsureGatewayKcp/>
            <EnsureRoleKcp/>
        </ReactiveSequence>
    </BehaviorTree>
</root>)";
		}
	}

	actor_robot::actor_robot(i16_area aarea, i32_actordataid arobotid, void*) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROBOT,
					.m_area = aarea,
					.m_id = arobotid,
				},
				.m_weight = 0x7fffffff,
			})
	{
		if (aarea != tab_self_area)
		{
			tools::no_core_dump();
			return;
		}
	}

	ENUM_ACTOR actor_robot::actor_type()
	{
		return ACTOR_ROBOT;
	}

	i64_actorid actor_robot::actorid(int32_t adataid)
	{
		return nguid::make(actor_type(), tab_self_area, adataid);
	}

	void actor_robot::init()
	{
		init_behavior_tree();

		np_timerparm ltimerparm;
		if (!make_timerparm::make_interval(ltimerparm, erobot_bt_tick_interval_sec))
		{
			log_error()->print("actor_robot::init() make_timerparm::make_interval({}, {}) failed", id_guid(), erobot_bt_tick_interval_sec);
			return;
		}
		set_timer(ltimerparm);
	}

	void actor_robot::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_robot::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_robot::nregister()
	{
		actor::register_timer<actor_robot>(&actor_robot::timer_handle);

		register_handle<actor_robot
			, np_thruput_test
			, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE
		>();

		nforward::g2c();
	}

	void actor_robot::init_behavior_tree()
	{
		m_behavior_factory.register_condition("HasSession",
			[this](BT::TreeNode&)
			{
				return m_session > 0 ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
			}
		);
		m_behavior_factory.register_condition("HasRoleSync",
			[this](BT::TreeNode&)
			{
				return m_firstsync ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
			}
		);
		m_behavior_factory.register_action("EnsureGatewayKcp",
			[this](BT::TreeNode&)
			{
				return ensure_kcp_connected(pbnet::ENUM_KCP::KCP_GATEWAY);
			}
		);
		m_behavior_factory.register_action("EnsureRoleKcp",
			[this](BT::TreeNode&)
			{
				return ensure_kcp_connected(pbnet::ENUM_KCP::KCP_ROLE);
			}
		);
		m_behavior_tree = m_behavior_factory.create_from_text(robot_behavior_tree_xml(), BT::Blackboard::create());
		sync_behavior_blackboard();
	}

	BT::Blackboard::Ptr actor_robot::behavior_blackboard()
	{
		if (m_behavior_tree.empty())
		{
			return nullptr;
		}
		return m_behavior_tree.global_blackboard();
	}

	void actor_robot::sync_behavior_blackboard()
	{
		BT::Blackboard::Ptr lblackboard = behavior_blackboard();
		if (lblackboard == nullptr)
		{
			return;
		}

		lblackboard->set("has_session", m_session > 0);
		lblackboard->set("role_synced", m_firstsync);
		lblackboard->set("gateway_kcp_requested", m_gateway_kcp_requested);
		lblackboard->set("role_kcp_requested", m_role_kcp_requested);
		lblackboard->set("gateway_kcp_connected", is_kcp_connected(pbnet::ENUM_KCP::KCP_GATEWAY));
		lblackboard->set("role_kcp_connected", is_kcp_connected(pbnet::ENUM_KCP::KCP_ROLE));
		lblackboard->set("robot_actorid", id_guid());
		if (m_robot != nullptr)
		{
			lblackboard->set("account", m_robot->m_account);
			lblackboard->set("gateway_serverid", m_robot->m_gatewayid);
			lblackboard->set("game_serverid", m_robot->m_gameid);
		}
	}

	bool actor_robot::is_kcp_connected(pbnet::ENUM_KCP akcpenum)
	{
		if (m_robot == nullptr)
		{
			return false;
		}

		i32_serverid lserverid = 0;
		switch (akcpenum)
		{
		case pbnet::ENUM_KCP::KCP_GATEWAY:
			lserverid = m_robot->m_gatewayid;
			break;
		case pbnet::ENUM_KCP::KCP_ROLE:
			lserverid = m_robot->m_gameid;
			break;
		default:
			return false;
		}
		if (lserverid <= 0)
		{
			return false;
		}

		auto luport = kcp_index(lserverid, akcpenum);
		if (!luport.has_value())
		{
			return false;
		}

		auto lpukcp = nkcp::instance().kcp(*luport);
		if (lpukcp == nullptr)
		{
			return false;
		}
		return lpukcp->find_session(id_guid()) != -1;
	}

	ai::bt_status actor_robot::ensure_kcp_connected(pbnet::ENUM_KCP akcpenum)
	{
		bool* lrequested = nullptr;
		int64_t* lrequestms = nullptr;
		switch (akcpenum)
		{
		case pbnet::ENUM_KCP::KCP_GATEWAY:
			lrequested = &m_gateway_kcp_requested;
			lrequestms = &m_gateway_kcp_request_ms;
			break;
		case pbnet::ENUM_KCP::KCP_ROLE:
			lrequested = &m_role_kcp_requested;
			lrequestms = &m_role_kcp_request_ms;
			break;
		default:
			return BT::NodeStatus::FAILURE;
		}

		if (is_kcp_connected(akcpenum))
		{
			sync_behavior_blackboard();
			return BT::NodeStatus::SUCCESS;
		}
		if (m_robot == nullptr || m_session <= 0)
		{
			return BT::NodeStatus::FAILURE;
		}

		const int64_t lnow = tools::time_wheel::getms();
		if (*lrequested && lnow - *lrequestms < erobot_kcp_retry_interval_ms)
		{
			sync_behavior_blackboard();
			return BT::NodeStatus::RUNNING;
		}

		ukcp_connect(akcpenum);
		*lrequested = true;
		*lrequestms = lnow;
		sync_behavior_blackboard();
		return BT::NodeStatus::RUNNING;
	}

	ai::bt_status actor_robot::tick_behavior_tree()
	{
		if (m_behavior_tree.empty())
		{
			return BT::NodeStatus::IDLE;
		}
		sync_behavior_blackboard();
		return m_behavior_tree.tick_once();
	}

	void actor_robot::ukcp_connect(pbnet::ENUM_KCP akcpenum)
	{
		if (m_robot == nullptr)
		{
			return;
		}

		int16_t lservertid = 0;
		int16_t ltcount = 0;
		i64_actorid lactorid = nguid::make();
		switch (akcpenum)
		{
		case pbnet::ENUM_KCP::KCP_GATEWAY:
		{
			lservertid = nnodeid::tid(m_robot->m_gatewayid);
			ltcount = nnodeid::tcount(m_robot->m_gatewayid);
		}
		break;
		case pbnet::ENUM_KCP::KCP_ROLE:
		{
			lservertid = nnodeid::tid(m_robot->m_gameid);
			ltcount = nnodeid::tcount(m_robot->m_gameid);
			lactorid = nguid::make_type(id_guid(), ACTOR_ROLE);
		}
		break;
		}
		std::string lcmd = std::format("kcp {} {} {} {} {}", (int32_t)akcpenum, lservertid, ltcount, lactorid, id_guid());
		ngl::actor_robot_manage::parse_command(std::move(lcmd));
	}

	bool actor_robot::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		tick_behavior_tree();
		return true;
	}
}//namespace ngl
