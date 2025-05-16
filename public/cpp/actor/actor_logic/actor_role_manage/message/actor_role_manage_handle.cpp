#include "actor_role_manage.h"
#include "actor_keyvalue.h"
#include "actor_create.h"
#include "nregister.h"
#include "ntimer.h"
namespace ngl
{
	bool actor_role_manage::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}

		if (handle_gm::empty())
		{
			handle_gm::add("roleban") = [this](int id, const ngl::json_read& aos)
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
				};

			handle_gm::add("get_roleban") = [this](int id, const ngl::json_read&)
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
}//namespace ngl
