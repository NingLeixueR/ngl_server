#include "actor_brief.h"
#include "net.pb.h"
#include "nsp_server.h"

namespace ngl
{
	actor_brief::actor_brief() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_BRIEF,
						.m_area = tab_self_area,
						.m_manage_dbclient = true
					},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		tdb_brief::nsp_ser::init(&m_briefdb);
	}

	ENUM_ACTOR actor_brief::actor_type()
	{
		return ACTOR_BRIEF;
	}

	i64_actorid actor_brief::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_brief::init()
	{
		// 绑定DB结构:DB.set(this);
		m_briefdb.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_brief::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_brief::loaddb_finish(bool adbishave)
	{
	}

	void actor_brief::nregister()
	{
		// 定时器
		actor::register_timer<actor_brief>(&actor_brief::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_brief>::func<
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_brief>::func<
		>(true);
	}
	
	bool actor_brief::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_brief::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl