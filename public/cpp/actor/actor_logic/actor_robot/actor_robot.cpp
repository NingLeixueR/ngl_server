#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
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
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_robot::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_robot::loaddb_finish(bool adbishave)
	{
	}

	void actor_robot::nregister()
	{
		// 定时器
		actor::register_timer<actor_robot>(&actor_robot::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_robot>::func<
			np_thruput_test
			, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE
		>(false);

		nforward::g2c();
	}

	const char* actor_robot::kcp_session()
	{
		return m_kcpsessionmd5.c_str();
	}

	bool actor_robot::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_robot::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl