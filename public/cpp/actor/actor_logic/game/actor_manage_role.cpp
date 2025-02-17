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

	bool actor_manage_role::handle(const message<np_roleban>& adata)
	{
		m_roleban = adata.get_data()->m_roleban;
		return true;
	}

	bool actor_manage_role::handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		auto recv = adata.get_data();

		if (m_roleban.contains(recv->m_roleid()))
		{
			return true;
		}

		nguid lguid(recv->m_roleid());
		log_error()->print("actor_manage_role roleid:{}", recv->m_roleid());
		np_actorswitch_process_role pro
		{
			.m_create = recv->m_iscreate(),
			.m_gatewayid = recv->m_gatewayid(),
		};
		actor_create::switch_process(recv->m_roleid(), 0, nconfig::m_nodeid, pro);
		return true;
	}

	bool actor_manage_role::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}

		if (handle_gm::empty())
		{
			handle_gm::push("roleban", [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "roleban", false);
					struct banrole
					{
						int64_t m_roleid = 0;
						int32_t m_stat = 0;
						jsonfunc("roleid", m_roleid, "stat", m_stat)
					};
					banrole m_banrole;
					if (aos.read("data", m_banrole) == false)
					{
						return;
					}

					if (m_banrole.m_stat == 1)
					{
						m_roleban.insert(m_banrole.m_roleid);
					}
					else
					{
						m_roleban.erase(m_banrole.m_roleid);
					}

					std::string lbanrole;
					tools::splicing(m_roleban, "*", lbanrole);
					
					pbdb::db_keyvalue* lpdata = nclient_keyvalue::add(pbdb::db_keyvalue_ekv_account_ban);
					lpdata->set_m_id(pbdb::db_keyvalue_ekv_account_ban);
					lpdata->set_m_value(lbanrole);
					nclient_keyvalue::change(pbdb::db_keyvalue_ekv_account_ban);
					pro.m_data = true;
				}
			);

			handle_gm::push("get_roleban", [this](int id, const ngl::json_read&)
				{
					gcmd<std::string> pro(id, "get_roleban");
					tools::splicing(m_roleban, "*", pro.m_data);
				}
			);
		}
		if (handle_gm::function(loperator, (int)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_role php operator[{}] ERROR", loperator);
		}
		return true;
	}
}//namespace ngl