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
#include "actor_ranklist.h"
#include "actor_brief.h"

namespace ngl
{
	actor_ranklist::actor_ranklist() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_RANKLIST,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_ranklist::actor_type()
	{
		return ACTOR_RANKLIST;
	}

	i64_actorid actor_ranklist::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_ranklist::init()
	{
		// 绑定DB结构:DB.set(this);
		m_ranklist.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_ranklist::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
		
	}

	void actor_ranklist::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_ranklist>::instance(id_guid()).exit();
	}

	void actor_ranklist::loaddb_finish(bool adbishave)
	{
	}

	void actor_ranklist::nregister()
	{
		// 定时器
		actor::register_timer<actor_ranklist>(&actor_ranklist::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_ranklist>::func<
			mforward<np_gm>
			, np_activityrank_operator
			, np_get_rank
			, mforward<pbnet::PROBUFF_NET_RANKLIST>
		>(false);
	}

	bool actor_ranklist::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_ranklist::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl