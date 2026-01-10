/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

	void actor_csvserver::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_csvserver::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_csvserver::nregister()
	{
		// 定时器
		actor::register_timer<actor_csvserver>(&actor_csvserver::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_csvserver
			, np_actor_csv_verify_version
		>(e_ready_all);
	}

	bool actor_csvserver::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl