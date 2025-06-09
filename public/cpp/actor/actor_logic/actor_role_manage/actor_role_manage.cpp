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
					.m_id = ttab_servers::tab()->m_tcount
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
		return nguid::make(actor_type(), nguid::none_area(), ttab_servers::tab()->m_tcount);
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


		tdb_keyvalue::nsp_cli<actor_role_manage>::getInstance(id_guid(), true).init(this, { pbdb::db_keyvalue_ekv_account_ban });
		tdb_keyvalue::nsp_cli<actor_role_manage>::getInstance(id_guid()).set_changedata_fun(
			[this](int64_t aid, const pbdb::db_keyvalue& akeyval, bool afirstsynchronize)
			{
				log_error()->print(
					"actor_manage_role nclient_keyvalue::set_changedata_fun####### [{}:{}:{}]",
					aid, akeyval.m_value().c_str(), afirstsynchronize ? "first" : "change"
				);
				if (afirstsynchronize)
				{
					tools::splite(akeyval.m_value().c_str(), "*", m_roleban);
				}
			});
	}

	void actor_role_manage::loaddb_finish(bool adbishave)
	{
	}

	void actor_role_manage::nregister()
	{
		// 定时器
		actor::register_timer<actor_role_manage>(&actor_role_manage::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_role_manage>::func<
			mforward<np_gm>
		>(false);

		// 绑定pb消息
		register_handle_proto<actor_role_manage>::func<
			pbnet::PROBUFF_NET_ROLE_LOGIN
		>(false);
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