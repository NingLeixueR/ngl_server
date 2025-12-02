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
#include "actor_testlua.h"

namespace ngl
{
	actor_testlua::actor_testlua() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_TESTLUA,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
					.m_enscript = enscript_lua,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_testlua::actor_type()
	{
		return ACTOR_TESTLUA;
	}

	i64_actorid actor_testlua::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_testlua::init()
	{
		// 绑定DB结构:DB.set(this);
		m_testlua.set(this);

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

		
		//tdb_brief::nsp_cread<actor_testlua>::instance_readpart(this, { ngl::tools::nguidstr2int64("actor_brief#1#1") });
	}

	void actor_testlua::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_testlua>::instance(id_guid()).exit();
	}
	
	void actor_testlua::loaddb_finish(bool adbishave)
	{
	}

	void actor_testlua::nregister()
	{
		// 定时器
		actor::register_timer<actor_testlua>(&actor_testlua::timer_handle);

		register_script_handle<actor_testlua>::func<
			np_testlua
		>(nready::e_ready_all);
		register_script_handle<actor_testlua>::func<
			mforward<pbnet::PROBUFF_NET_TESTLUA>
		>(nready::e_ready_all);
	}

	bool actor_testlua::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_testlua::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl