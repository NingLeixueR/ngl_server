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
#include "actor_testlua2.h"

namespace ngl
{
	actor_testlua2::actor_testlua2() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_TESTLUA2,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
					.m_enscript = enscript_lua,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_testlua2::actor_type()
	{
		return ACTOR_TESTLUA2;
	}

	i64_actorid actor_testlua2::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_testlua2::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_testlua::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		// # 将csv数据写入lua
		//nscript_data_csv<tab_servers> ltab(ttab_servers::instance().tablecsv());
		//nscript_data_push("csv", ltab);
	}

	void actor_testlua2::erase_actor_before()
	{
	}
	
	void actor_testlua2::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_testlua2::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_testlua2::nregister()
	{
		// 定时器
		actor::register_timer<actor_testlua2>(&actor_testlua2::timer_handle);

		register_script_handle<actor_testlua2
			, np_testlua
		>(e_ready_all);
	}

	bool actor_testlua2::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}// namespace ngl