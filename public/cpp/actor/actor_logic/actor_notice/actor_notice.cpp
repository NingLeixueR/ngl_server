#include "actor_notice.h"

namespace ngl
{
	actor_notice::actor_notice() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_NOTICE,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_notice::actor_type()
	{
		return ACTOR_NOTICE;
	}

	i64_actorid actor_notice::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_notice::init()
	{
		// 绑定DB结构:DB.set(this);
		m_notice.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_notice::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_notice::loaddb_finish(bool adbishave) 
	{
	}

	void actor_notice::nregister()
	{
		// 定时器
		actor::register_timer<actor_notice>(&actor_notice::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_notice, false>::func<
			mforward<np_gm>,
			np_actor_addnotice
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_notice, false>::func<
			mforward<pbnet::PROBUFF_NET_NOTICE>
		>(true);
	}

	bool actor_notice::sendnotice(const std::string& anotice, int32_t astarttime, int32_t afinishtime)
	{
		auto pro = std::make_shared<np_actor_addnotice>();
		pro->m_notice = anotice;
		pro->m_starttime = astarttime;
		pro->m_finishtime = afinishtime;
		actor::static_send_actor(actorid(), nguid::make(), pro);
		return true;
	}

	bool actor_notice::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_notice::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl