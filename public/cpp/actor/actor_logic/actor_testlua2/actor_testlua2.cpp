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
		nscript_push_csv<ttab_servers>();
	}

	void actor_testlua2::erase_actor_before()
	{
	}
	
	void actor_testlua2::loaddb_finish(bool adbishave)
	{
	}

	void actor_testlua2::nregister()
	{
		// 定时器
		actor::register_timer<actor_testlua>(&actor_testlua::timer_handle);

		register_script_handle<EPROTOCOL_TYPE_CUSTOM, actor_testlua>::func<
			np_testlua
		>(true);
	}

	bool actor_testlua2::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_testlua2::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl