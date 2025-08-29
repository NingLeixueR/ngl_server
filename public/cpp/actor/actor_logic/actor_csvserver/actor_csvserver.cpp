#include "actor_csvserver.h"

namespace ngl
{
	actor_csvserver::actor_csvserver() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVSERVER,
				},
				.m_weight = 0x7fffffff,
			})
	{}

	ENUM_ACTOR actor_csvserver::actor_type()
	{
		return ACTOR_CSVSERVER;
	}

	i64_actorid actor_csvserver::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_csvserver::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_csvserver::loaddb_finish(bool adbishave)
	{
	}

	void actor_csvserver::nregister()
	{
		// 定时器
		actor::register_timer<actor_csvserver>(&actor_csvserver::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_csvserver>::func<
			np_actor_csv_verify_version
		>(true);

		// 绑定pb消息
		register_handle<actor_csvserver>::func<
		>(true);
	}

	bool actor_csvserver::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_csvserver::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl