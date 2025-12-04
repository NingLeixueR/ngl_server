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
#include "actor_family.h"

namespace ngl
{
	actor_family::actor_family() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_FAMILY,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_family::actor_type()
	{
		return ACTOR_FAMILY;
	}

	i64_actorid actor_family::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_family::init()
	{
		// 绑定DB结构:DB.set(this);
		m_familyer.set(this);
		m_family.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_family::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_family>::instance_readall(this, pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase"));
		m_drop.init(this, {});

	}

	void actor_family::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_family>::instance(id_guid()).exit();
		m_drop.exit();
	}

	void actor_family::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_family::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
		actor_events_logic::tfun::func<>(
			actorid()
			, eevents_logic_rolelogin
			, eevents_logic_roleoffline
		);
	}

	void actor_family::nregister()
	{
		// 定时器
		actor::register_timer<actor_family>(&actor_family::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_family>::func<
			mforward<np_gm>
			, np_eevents_logic_rolelogin
			, np_eevents_logic_roleoffline
			, mforward<pbnet::PROBUFF_NET_FAMIL_CREATE>
			, mforward<pbnet::PROBUFF_NET_FAMIL_JOIN>
			, mforward<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>
			, mforward<pbnet::PROBUFF_NET_FAMIL_CEDE>
			, mforward<pbnet::PROBUFF_NET_FAMIL_LEAVE>
			, mforward<pbnet::PROBUFF_NET_FAMIL_LIST>
			, mforward<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>
			, mforward<pbnet::PROBUFF_NET_FAMIL_SIGN>
			, mforward<pbnet::PROBUFF_NET_FAMIL_INFO>
		>(nready::e_ready_all);
	}

	bool actor_family::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_family::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl