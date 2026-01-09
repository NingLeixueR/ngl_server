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
#include "actor_brief.h"
#include "net.pb.h"

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

		tdb_brief::nsp_ser::init(&m_briefdb);
	}

	void actor_brief::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_brief::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
		log_error()->print("actor_brief::loaddb_finish({})", astat == enum_dbstat_success ?"load":"create");
	}

	void actor_brief::nregister()
	{
		// 定时器
		actor::register_timer<actor_brief>(&actor_brief::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_brief>::func<
		>(e_ready_all);
	}
	
	bool actor_brief::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl