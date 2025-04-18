#include "actor_keyvalue.h"
#include "nsp_server.h"
#include "net.pb.h"

namespace ngl
{
	actor_keyvalue::actor_keyvalue() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_KEYVALUE,
						.m_area = tab_self_area,
						.m_manage_dbclient = true
					},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		tdb_keyvalue::nsp_ser::init(&m_keyvaluedb);
	}

	ENUM_ACTOR actor_keyvalue::actor_type()
	{
		return ACTOR_KEYVALUE;
	}

	i64_actorid actor_keyvalue::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_keyvalue::init()
	{
		// 绑定DB结构:DB.set(this);
		m_keyvaluedb.set(this);

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

	void actor_keyvalue::loaddb_finish(bool adbishave)
	{
		log_error()->print("actor_keyvalue load finish!!!");
	}

	void actor_keyvalue::nregister()
	{
		// 定时器
		actor::register_timer<actor_keyvalue>(&actor_keyvalue::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_keyvalue>::func<
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_keyvalue>::func<
		>(true);
	}

	bool actor_keyvalue::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_keyvalue::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl