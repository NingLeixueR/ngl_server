#include "actor_csvclient.h"
#include "actor_csvserver.h"
#include "actor_manage.h"

namespace ngl
{
	actor_csvclient::actor_csvclient() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVCLIENT,
					.m_area = tab_self_area,
					.m_id	= nconfig::m_nodeid
				},
				.m_weight = 0x7fffffff,
			})
	{}

	ENUM_ACTOR actor_csvclient::actor_type()
	{
		return ACTOR_CSVCLIENT;
	}

	i64_actorid actor_csvclient::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_csvclient::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 5 * 60) == false)
		{
			log_error()->print("actor_csvclient::init() make_timerparm::make_interval(tparm, 60) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	void actor_csvclient::loaddb_finish(bool adbishave)
	{
	}

	void actor_csvclient::nregister()
	{
		// 定时器
		actor::register_timer<actor_csvclient>(&actor_csvclient::timer_handle);
		
		// 绑定自定义np_消息
		register_handle<actor_csvclient>::func<
			np_actor_reloadcsv
		>(false);
	}

	bool actor_csvclient::timer_handle(const message<np_timerparm>&)
	{
		log_error()->print("############actor_csvclient::timer_handle###########");
		auto pro = std::make_shared<np_actor_csv_verify_version>();
		allcsv::foreach_verify(pro->m_verify);
		send_actor(actor_csvserver::actorid(), id_guid(), pro);
		return true;
	}

	bool actor_csvclient::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl