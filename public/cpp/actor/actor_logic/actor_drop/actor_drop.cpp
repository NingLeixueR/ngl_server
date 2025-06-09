#include "actor_drop.h"
#include "actor_chat.h"
#include "net.pb.h"

namespace ngl
{
	actor_drop::actor_drop() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_DROP,
						.m_area = tab_self_area,
						.m_id = nconfig::m_nodeid
					},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_drop::actor_type()
	{
		return ACTOR_DROP;
	}

	i64_actorid actor_drop::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nconfig::m_nodeid);
	}

	void actor_drop::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		//np_timerparm tparm;
		//if (make_timerparm::make_interval(tparm, 2) == false)
		//{
		//	log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
		//	return;
		//}
		//set_timer(tparm);

		m_drop.init(this, {});
	}

	void actor_drop::erase_actor_before()
	{
		m_drop.exit();
	}

	void actor_drop::loaddb_finish(bool adbishave)
	{
	}

	void actor_drop::nregister()
	{
		// 定时器
		//actor::register_timer<actor_drop>(&actor_drop::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_chat>::func<
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_chat>::func<
		>(true);
	}
	
	bool actor_drop::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_drop::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl