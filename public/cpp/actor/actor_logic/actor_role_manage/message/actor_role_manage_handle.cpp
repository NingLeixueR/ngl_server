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
	bool actor_role_manage::handle(const message<mforward<np_gm>>& adata)
	{
		ncjson lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::pop(lojson.json(), { "operator" }, loperator))
		{
			return true;
		}

		if (handle_gm::empty())
		{
			handle_gm::add("roleban") = [this](int id, ncjson& aos)
				{
					gcmd<bool> pro(id, "roleban", false);
					struct banrole
					{
						int64_t m_roleid = 0;
						int32_t m_stat = 0;
						DPROTOCOL(banrole, m_roleid, m_stat)
					};
					banrole lbanrole;
					if (!njson::pop(aos.json(), { "data" }, lbanrole))
					{
						return;
					}

					if (lbanrole.m_stat == 1)
					{
						m_roleban.insert(lbanrole.m_roleid);
					}
					else
					{
						m_roleban.erase(lbanrole.m_roleid);
					}

					std::string lbanrolestr;
					tools::splicing(m_roleban, "*", lbanrolestr);

					pbdb::db_keyvalue* lpdata = tdb_keyvalue::nsp_cwrite<actor_role_manage>::instance(id_guid()).add(pbdb::db_keyvalue_ekv_account_ban);
					lpdata->set_mid(pbdb::db_keyvalue_ekv_account_ban);
					lpdata->set_mvalue(lbanrolestr);
					tdb_keyvalue::nsp_cwrite<actor_role_manage>::instance(id_guid()).change();
					pro.m_data = true;
				};

			handle_gm::add("get_roleban") = [this](int id, ncjson&)
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
			.m_gatewayid = recv->mgatewayid(),
		};
		actor_create::switch_process(recv->mroleid(), 0, nconfig.nodeid(), pro);
		return true;
	}
}//namespace ngl