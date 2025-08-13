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
		nscript_data_csv<tab_servers> ltab(ttab_servers::instance().tablecsv());
		nscript_data_push("csv", ltab);

		
		tdb_brief::nsp_cli<actor_testlua>::instance(id_guid(), true).init_parts(this, 
			{
				ngl::tools::nguidstr2int64("actor_brief#1#1")
			});
	}

	void actor_testlua::erase_actor_before()
	{
		tdb_brief::nsp_cli<actor_testlua>::instance(id_guid()).exit();
		tdb_brief::nsp_cli<actor_testlua>::freensp(id_guid());
	}
	
	void actor_testlua::loaddb_finish(bool adbishave)
	{
	}

	void actor_testlua::nregister()
	{
		// 定时器
		actor::register_timer<actor_testlua>(&actor_testlua::timer_handle);

		register_script_handle<EPROTOCOL_TYPE_CUSTOM, actor_testlua>::func<
			np_testlua
		>(true);
		register_script_handle<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_testlua>::func<
			mforward<pbnet::PROBUFF_NET_TESTLUA>
		>(true);
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