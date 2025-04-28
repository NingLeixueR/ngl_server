#include "calendar_function.h"
#include "actor_calendar.h"
#include "ttab_calendar.h"

namespace ngl
{
	actor_calendar::actor_calendar() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CALENDAR,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_calendar::actor_type()
	{
		return ACTOR_CALENDAR;
	}

	i64_actorid actor_calendar::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_calendar::init()
	{
		// 绑定DB结构:DB.set(this);
		m_calendar.set(this);

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

	void actor_calendar::loaddb_finish(bool adbishave) 
	{
	}

	void actor_calendar::nregister()
	{
		// 定时器
		actor::register_timer<actor_calendar>(&actor_calendar::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_calendar>::func<
			np_calendar,
			mforward<np_actor_calendar_requst>
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_calendar>::func<
		>(true);
	}

	bool actor_calendar::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_calendar::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl