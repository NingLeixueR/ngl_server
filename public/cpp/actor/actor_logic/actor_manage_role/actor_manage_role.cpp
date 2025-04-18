#include "actor_manage_role.h"
#include "actor_keyvalue.h"
#include "actor_create.h"
#include "nregister.h"
#include "ntimer.h"

namespace ngl
{
	actor_manage_role::actor_manage_role() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MANAGE_ROLE,
					.m_area = tab_self_area,
					.m_id = ttab_servers::tab()->m_tcount
				},
				.m_weight = 0x7fffffff,
			})
	{
		std::set<i64_actorid> ldatakvid{ pbdb::db_keyvalue_ekv_account_ban };
		nclient_keyvalue::init(actor_keyvalue::actor_type(), this, ldatakvid);
		nclient_keyvalue::set_changedata_fun([this](int64_t aid, const pbdb::db_keyvalue& akeyval, bool afirstsynchronize)
			{
				log_error()->print(
					"actor_manage_role nclient_keyvalue::set_changedata_fun####### [{}:{}:{}]", 
					aid, akeyval.m_value().c_str(), afirstsynchronize?"first":"change"
				);

				auto pro = std::make_shared<np_roleban>();
				tools::splite(akeyval.m_value().c_str(), "*", pro->m_roleban);
				actor::static_send_actor(actor_manage_role::actorid(), nguid::make(), pro);
			});
	}

	ENUM_ACTOR actor_manage_role::actor_type()
	{
		return ACTOR_MANAGE_ROLE;
	}

	i64_actorid actor_manage_role::actorid()
	{
		return nguid::make(actor_type(), nguid::none_area(), ttab_servers::tab()->m_tcount);
	}

	void actor_manage_role::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_manage_role::loaddb_finish(bool adbishave)
	{
	}

	void actor_manage_role::nregister()
	{
		// 定时器
		actor::register_timer<actor_manage_role>(&actor_manage_role::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_manage_role>::func<
			mforward<np_gm>,
			np_roleban
		>(false);

		// 绑定pb消息
		register_handle_proto<actor_manage_role>::func<
			pbnet::PROBUFF_NET_ROLE_LOGIN
		>(false);
	}

	bool actor_manage_role::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_manage_role::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl