#include "actor_kcp.h"
#include "ukcp.h"

namespace ngl
{
	actor_kcp::actor_kcp():
		actor(
		actorparm
		{
			.m_parm
			{
				.m_type = ACTOR_KCP,
				.m_area = tab_self_area,
				.m_id = nconfig::m_nodeid,
			},
			.m_weight = 0x7fffffff
		})
	{

	}

	ENUM_ACTOR actor_kcp::actor_type()
	{
		return ACTOR_KCP;
	}

	i64_actorid actor_kcp::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_kcp::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_kcp::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_kcp::loaddb_finish(bool adbishave)
	{
	}

	void actor_kcp::nregister()
	{
		// 定时器
		actor::register_timer<actor_kcp>(&actor_kcp::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_kcp, false>::func<
			np_actor_kcp
		>(false);

		// 绑定pb消息
		register_handle_proto<actor_kcp, false>::func<
		>(true);
	}

	bool actor_kcp::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_kcp::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl