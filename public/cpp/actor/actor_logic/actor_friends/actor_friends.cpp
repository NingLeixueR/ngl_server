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
#include "actor_friends.h"
#include "actor_brief.h"

namespace ngl
{
	actor_friends::actor_friends() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_FRIENDS,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_friends::actor_type()
	{
		return ACTOR_FRIENDS;
	}

	i64_actorid actor_friends::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_friends::init()
	{
		// 绑定DB结构:DB.set(this);
		m_friends.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_friends::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_friends>::instance_readall(this, pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase"));
	}

	void actor_friends::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_friends>::instance(id_guid()).exit();
	}

	void actor_friends::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_friends::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
		actor_events_logic::event_func(actorid(), eevents_logic_rolelogin, eevents_logic_roleoffline);
	}

	void actor_friends::nregister()
	{
		// 定时器
		actor::register_timer<actor_friends>(&actor_friends::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_friends>::func<
			np_eevents_logic_rolelogin
			, np_eevents_logic_roleoffline
			, mforward<pbnet::PROBUFF_NET_FRIEND_ADD>
			, mforward<pbnet::PROBUFF_NET_FRIEND_ERASE>
			, mforward<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>
			, mforward<pbnet::PROBUFF_NET_FRIEND>
		>(e_ready_all);
	}

	bool actor_friends::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}// namespace ngl