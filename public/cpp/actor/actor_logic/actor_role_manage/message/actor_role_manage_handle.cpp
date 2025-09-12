#include "actor_role_manage.h"
#include "actor_keyvalue.h"
#include "actor_create.h"
#include "nregister.h"
#include "ntimer.h"
namespace ngl
{
	bool actor_role_manage::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::njson_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::read(lojson, "operator", loperator))
		{
			return true;
		}

		if (handle_gm::empty())
		{
			handle_gm::add("roleban") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<bool> pro(id, "roleban", false);
					struct banrole
					{
						int64_t m_roleid = 0;
						int32_t m_stat = 0;
						dprotocol(banrole, m_roleid, m_stat)
					};
					banrole m_banrole;
					if (!njson::read(aos, "data", m_banrole))
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

					pbdb::db_keyvalue* lpdata = tdb_keyvalue::nsp_cli<actor_role_manage>::instance(id_guid()).add(pbdb::db_keyvalue_ekv_account_ban);
					lpdata->set_mid(pbdb::db_keyvalue_ekv_account_ban);
					lpdata->set_mvalue(lbanrole);
					tdb_keyvalue::nsp_cli<actor_role_manage>::instance(id_guid()).change(pbdb::db_keyvalue_ekv_account_ban);
					pro.m_data = true;
				};

			handle_gm::add("get_roleban") = [this](int id, ngl::njson_read&)
				{
					gcmd<std::string> pro(id, "get_roleban");
					tools::splicing(m_roleban, "*", pro.m_data);
				};
		}
		if (handle_gm::function(loperator, (int)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_role php operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_role_manage::handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		auto recv = adata.get_data();

		if (m_roleban.contains(recv->mroleid()))
		{
			return true;
		}

		nguid lguid(recv->mroleid());
		log_error()->print("actor_manage_role roleid:{}", recv->mroleid());
		np_actorswitch_process_role pro
		{
			.m_create = recv->miscreate(),
			.m_gatewayid = recv->mgatewayid(),
		};
		actor_create::switch_process(recv->mroleid(), 0, nconfig::m_nodeid, pro);
		return true;
	}
}//namespace ngl
