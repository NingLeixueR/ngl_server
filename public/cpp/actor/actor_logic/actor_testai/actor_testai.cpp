#include "actor/actor_logic/actor_testai/actor_testai.h"
#include "actor/actor_base/core/nregister.h"

#include <random>

namespace ngl
{
	namespace
	{
		constexpr int32_t etestai_tick_interval_sec = 2;
		constexpr int32_t etestai_patrol_max = 5;

		std::mt19937& rng()
		{
			static std::mt19937 lrng(std::random_device{}());
			return lrng;
		}

		bool random_chance(int32_t apercent)
		{
			std::uniform_int_distribution<int32_t> ldist(1, 100);
			return ldist(rng()) <= apercent;
		}
	}

	actor_testai::actor_testai(i16_area aarea, i32_actordataid aid) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_TESTAI,
					.m_area = aarea,
					.m_id = aid,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_testai::actor_type()
	{
		return ACTOR_TESTAI;
	}

	i64_actorid actor_testai::actorid(int32_t adataid)
	{
		return nguid::make(actor_type(), tab_self_area, adataid);
	}

	void actor_testai::init()
	{
		register_bt_nodes();

		m_bt.init(this);
		m_bt.load_tree("testai_main");
		m_bt.set<int32_t>("patrol_index", 0);
		m_bt.set<int32_t>("hp", m_hp);

		np_timerparm ltimerparm;
		if (!make_timerparm::make_interval(ltimerparm, etestai_tick_interval_sec))
		{
			log_error()->print("actor_testai::init() make_timerparm failed");
			return;
		}
		set_timer(ltimerparm);

		log_error()->print("[TESTAI:{}] behavior tree initialized, tick interval={}s", id_guid(), etestai_tick_interval_sec);
	}

	void actor_testai::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_testai::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_testai::nregister()
	{
		actor::register_timer<actor_testai>(&actor_testai::timer_handle);
	}

	void actor_testai::register_bt_nodes()
	{
		static bool lregistered = false;
		if (lregistered)
		{
			return;
		}
		lregistered = true;

		auto& lfactory = ai2::nbt_factory::instance();

		// condition: 30% 概率发现敌人
		lfactory.register_condition("CheckEnemy",
			[](ai2::nbt_context& actx, BT::TreeNode&) -> ai2::nbt_status
			{
				auto* lactor = static_cast<actor_testai*>(actx.get_actor());
				bool lfound = random_chance(30);
				if (lfound)
				{
					log_error()->print("[TESTAI:{}] detected enemy nearby!", lactor->id_guid());
				}
				return lfound ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
			});

		// action: 攻击
		lfactory.register_action("Attack",
			[](ai2::nbt_context& actx, BT::TreeNode&) -> ai2::nbt_status
			{
				auto* lactor = static_cast<actor_testai*>(actx.get_actor());
				int32_t ldamage = std::uniform_int_distribution<int32_t>(10, 30)(rng());
				log_error()->print("[TESTAI:{}] attacking enemy! damage={}", lactor->id_guid(), ldamage);
				return BT::NodeStatus::SUCCESS;
			});

		// condition: 检查是否有巡逻点
		lfactory.register_condition("CheckPatrolPoint",
			[](ai2::nbt_context& actx, BT::TreeNode&) -> ai2::nbt_status
			{
				int32_t lidx = actx.get<int32_t>("patrol_index");
				return lidx < etestai_patrol_max ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
			});

		// action: 巡逻
		lfactory.register_action("Patrol",
			[](ai2::nbt_context& actx, BT::TreeNode&) -> ai2::nbt_status
			{
				auto* lactor = static_cast<actor_testai*>(actx.get_actor());
				int32_t lidx = actx.get<int32_t>("patrol_index");
				lidx = (lidx + 1) % etestai_patrol_max;
				actx.set<int32_t>("patrol_index", lidx);
				log_error()->print("[TESTAI:{}] patrolling to point {}/{}", lactor->id_guid(), lidx, etestai_patrol_max);
				return BT::NodeStatus::SUCCESS;
			});

		// action: 待机
		lfactory.register_action("Idle",
			[](ai2::nbt_context& actx, BT::TreeNode&) -> ai2::nbt_status
			{
				auto* lactor = static_cast<actor_testai*>(actx.get_actor());
				log_error()->print("[TESTAI:{}] idle... waiting", lactor->id_guid());
				return BT::NodeStatus::SUCCESS;
			});

		// 注册行为树 XML
		const std::string lxml = R"(
<root BTCPP_format="4" main_tree_to_execute="testai_main">
  <BehaviorTree ID="testai_main">
    <Fallback>
      <Sequence>
        <CheckEnemy/>
        <Attack/>
      </Sequence>
      <Sequence>
        <CheckPatrolPoint/>
        <Patrol/>
      </Sequence>
      <Idle/>
    </Fallback>
  </BehaviorTree>
</root>
)";
		lfactory.register_tree("testai_main", lxml);
	}

	bool actor_testai::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		ai2::nbt_status lstatus = m_bt.tick();
		log_error()->print("[TESTAI:{}] tick result={}", id_guid(), BT::toStr(lstatus));
		return true;
	}
}// namespace ngl
