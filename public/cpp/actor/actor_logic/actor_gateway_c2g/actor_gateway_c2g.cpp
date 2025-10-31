#include "actor_gateway_c2g.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_gateway_c2g::actor_gateway_c2g() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_C2G,
					.m_area = tab_self_area,
					.m_id = nconfig::m_tcount,
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	ENUM_ACTOR actor_gateway_c2g::actor_type()
	{
		return ACTOR_GATEWAY_C2G;
	}

	i64_actorid actor_gateway_c2g::actorid(i32_actordataid aactordataid)
	{
		return nguid::make(ACTOR_GATEWAY_C2G, tab_self_area, aactordataid);
	}

	void actor_gateway_c2g::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时�?
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gateway_c2g::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_gateway_c2g::loaddb_finish(bool adbishave)
	{
	}

	void actor_gateway_c2g::nregister()
	{
		// 定时�?
		actor::register_timer<actor_gateway_c2g>(&actor_gateway_c2g::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_gateway_c2g>::func<
			np_actor_gatewayinfo_updata
		>(false);

		// 绑定pb消息
		nforward::c2g();
	}

	bool actor_gateway_c2g::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_gateway_c2g::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl