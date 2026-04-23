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
// File overview: Implements robot actor construction and simulated player behavior.


#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "actor/actor_logic/actor_robot/actor_robot.h"
#include "actor/actor_base/core/nregister.h"
#include "actor/actor_logic/nforward.h"
#include "tools/tools/tools_file.h"
#include "net/udp/kcp/nkcp.h"

namespace ngl
{
	namespace
	{
		constexpr int64_t erobot_kcp_retry_interval_ms = 3 * tools::time::MILLISECOND;
	}

	nbt_factory<actor_robot> actor_robot::m_factory;

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
		m_bt.init(this, &m_factory);
		m_bt.load_tree("actor_robot");

		np_timerparm ltimerparm;
		if (!make_timerparm::make_interval(ltimerparm, 1))
		{
			log_error()->print("actor_robot::init() make_timerparm failed");
			return;
		}
		set_timer(ltimerparm);

		log_error()->print("[ROBOT:{}] behavior tree initialized, tick interval={}s", id_guid(), 1);
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

		static std::once_flag lfirst;
		std::call_once(lfirst, []()
			{
				m_factory.register_condition("HasSession",
					[](nbt_context<actor_robot>& actx, BT::TreeNode&)
					{
						auto lpactor = actx.get_actor();
						if (lpactor == nullptr)
						{
							return BT::NodeStatus::FAILURE;
						}
						return lpactor->m_session > 0 ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
					}
				);
				m_factory.register_condition("HasRoleSync",
					[](nbt_context<actor_robot>& actx, BT::TreeNode&)
					{
						auto lpactor = actx.get_actor();
						if (lpactor == nullptr)
						{
							return BT::NodeStatus::FAILURE;
						}
						return lpactor->m_firstsync ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
					}
				);
				m_factory.register_stateful_action("EnsureGatewayKcp",
					[](nbt_context<actor_robot>& actx, BT::TreeNode&)
					{
						auto lpactor = actx.get_actor();
						if (lpactor == nullptr)
						{
							return BT::NodeStatus::FAILURE;
						}
						return lpactor->ensure_kcp_connected(pbnet::ENUM_KCP::KCP_GATEWAY);
					}
				);
				m_factory.register_stateful_action("EnsureRoleKcp",
					[](nbt_context<actor_robot>& actx, BT::TreeNode&)
					{
						auto lpactor = actx.get_actor();
						if (lpactor == nullptr)
						{
							return BT::NodeStatus::FAILURE;
						}
						return lpactor->ensure_kcp_connected(pbnet::ENUM_KCP::KCP_ROLE);
					}
				);
				// 注册行为树 XML		
				m_factory.register_tree_file("config/ai/actor_robot.xml");
			}
		);
		
		
	}

	nbt_status actor_robot::is_kcp_connected(pbnet::ENUM_KCP akcpenum)
	{
		if (m_robot == nullptr)
		{
			return BT::NodeStatus::FAILURE;
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
			return BT::NodeStatus::FAILURE;
		}
		if (lserverid <= 0)
		{
			return BT::NodeStatus::FAILURE;
		}

		auto luport = kcp_index(lserverid, akcpenum);
		if (!luport.has_value())
		{
			return BT::NodeStatus::FAILURE;
		}

		auto lpukcp = nkcp::instance().kcp(*luport);
		if (lpukcp == nullptr)
		{
			return BT::NodeStatus::FAILURE;
		}
		return (lpukcp->find_session(id_guid()) != -1? BT::NodeStatus::SUCCESS: BT::NodeStatus::FAILURE);
	}

	nbt_status actor_robot::ensure_kcp_connected(pbnet::ENUM_KCP akcpenum)
	{
		if (is_kcp_connected(akcpenum) == BT::NodeStatus::SUCCESS)
		{
			return BT::NodeStatus::SUCCESS;
		}
		if (m_robot == nullptr || m_session <= 0)
		{
			return BT::NodeStatus::FAILURE;
		}
		ukcp_connect(akcpenum);
		return BT::NodeStatus::SUCCESS;
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
		if (m_status != BT::NodeStatus::SUCCESS)
		{
			m_status = m_bt.tick();
			log_error()->print("[ROBOT:{}] tick result={}", id_guid(), BT::toStr(m_status));
		}
		return true;
	}
}//namespace ngl
