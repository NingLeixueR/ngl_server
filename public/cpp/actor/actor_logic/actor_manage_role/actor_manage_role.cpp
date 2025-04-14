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

	void actor_manage_role::nregister()
	{
		register_handle_proto<actor_manage_role>::func<
			pbnet::PROBUFF_NET_ROLE_LOGIN
		>(false);

		register_handle_custom<actor_manage_role>::func<
			mforward<np_gm>,
			np_roleban
		>(false);
	}
}//namespace ngl