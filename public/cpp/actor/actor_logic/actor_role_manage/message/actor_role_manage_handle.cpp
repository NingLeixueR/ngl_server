/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements message handlers for message.
#include "actor/actor_logic/actor_role_manage/actor_role_manage.h"
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_base/nregister.h"
#include "actor/actor_base/ntimer.h"
namespace ngl
{
	bool actor_role_manage::handle(const message<mforward<np_gm>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		ncjson lojson(lrecv->m_json.c_str());
		std::string loperator;
		if (!njson::pop(lojson, { "operator" }, loperator))
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
					if (!njson::pop(aos, { "data" }, lbanrole))
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
		if (handle_gm::function(loperator, (int)lparm->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_role php operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_role_manage::handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		const auto* lrecv = adata.get_data();

		if (m_roleban.find(lrecv->mroleid()) != m_roleban.end())
		{
			return true;
		}

		nguid lguid(lrecv->mroleid());
		log_error()->print("actor_manage_role roleid:{}", lrecv->mroleid());
		np_actorswitch_process_role pro
		{
			.m_gatewayid = lrecv->mgatewayid(),
		};
		actor_create::switch_process(lrecv->mroleid(), 0, nconfig.nodeid(), pro);
		return true;
	}
}//namespace ngl
