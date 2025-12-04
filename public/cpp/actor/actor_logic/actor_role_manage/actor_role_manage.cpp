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
#include "actor_role_manage.h"
#include "actor_keyvalue.h"
#include "actor_create.h"
#include "nregister.h"
#include "ntimer.h"

namespace ngl
{
	actor_role_manage::actor_role_manage() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROLE_MANAGE,
					.m_area = tab_self_area,
					.m_id = nconfig::m_tcount
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_role_manage::actor_type()
	{
		return ACTOR_ROLE_MANAGE;
	}

	i64_actorid actor_role_manage::actorid()
	{
		return nguid::make(actor_type(), nguid::none_area(), nconfig::m_tcount);
	}

	void actor_role_manage::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_role_manage::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		std::set<i32_fieldnumber> lfieldset;
		tdb_keyvalue::nsp_cwrite<actor_role_manage>::instance_writepart(
			this
			, { }
			, pb_field::field_number<pbdb::db_keyvalue>(lfieldset, "mvalue")
			, { }
			, { pbdb::db_keyvalue_ekv_account_ban }		
		);
		
		tdb_keyvalue::nsp_cwrite<actor_role_manage>::instance(id_guid()).set_changedatafun( 
			[this](int64_t aid, const pbdb::db_keyvalue& akeyval, bool afirstsynchronize)
			{
				log_error()->print(
					"actor_manage_role nclient_keyvalue::set_changedata_fun####### [{}:{}:{}]"
					, aid, akeyval.mvalue().c_str()
					, afirstsynchronize ? "first" : "change"
				);
				if (afirstsynchronize)
				{
					tools::splite(akeyval.mvalue().c_str(), "*", m_roleban);
				}
			});
	}

	void actor_role_manage::handle_after(handle_pram&)
	{
		tdb_brief::nsp_cwrite<actor_role_manage>::change(id_guid());
	}

	void actor_role_manage::erase_actor_before()
	{
		tdb_keyvalue::nsp_cread<actor_role_manage>::instance(id_guid()).exit();
	}

	void actor_role_manage::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			return;
		}
	}

	void actor_role_manage::nregister()
	{
		// 定时器
		actor::register_timer<actor_role_manage>(&actor_role_manage::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_role_manage>::func<
			mforward<np_gm>
			, pbnet::PROBUFF_NET_ROLE_LOGIN
		>(nready::e_ready_all);
	}

	bool actor_role_manage::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_role_manage::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl