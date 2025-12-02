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
		register_handle<actor_notice>::func<
			mforward<np_gm>
			, np_actor_addnotice
			, mforward<pbnet::PROBUFF_NET_NOTICE>
		>(nready::e_ready_all);
	}

	bool actor_notice::sendnotice(const std::string& anotice, int32_t astarttime, int32_t afinishtime)
	{
		auto pro = std::make_shared<np_actor_addnotice>();
		pro->m_notice = anotice;
		pro->m_starttime = astarttime;
		pro->m_finishtime = afinishtime;
		actor::send_actor(actorid(), actor_notice::actorid(), pro);
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