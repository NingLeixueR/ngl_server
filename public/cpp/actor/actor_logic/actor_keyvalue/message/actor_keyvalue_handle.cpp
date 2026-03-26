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
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
namespace ngl
{
	bool actor_keyvalue::handle([[maybe_unused]] const message<mforward<np_gm>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		ncjson lojson(lrecv->m_json.c_str());
		std::string loperator;
		if (!njson::pop(lojson, { "operator" }, loperator))
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("set_openserver") = [this](int id, ncjson& aos)
				{
					gcmd<bool> pro(id, "set_openserver", false);
					int32_t ltime = 0;
					if (!njson::pop(aos, { "data" }, ltime))
					{
						return;
					}
					log_error()->print("set server time = [{}]", localtime::time2str(ltime));
					
					int32_t lopentime = 0;
					if (m_keyvaluedb.keyvalue(pbdb::db_keyvalue::open_server, lopentime) && lopentime != 0)
					{
						log_error()->print("set server fail time = [{}]", localtime::time2str(lopentime));
						return;
					}

					m_keyvaluedb.keyvalue_add(pbdb::db_keyvalue::open_server, ltime);
					pro.m_data = true;
				};

				handle_cmd::add("get_openserver") = [this](int id, [[maybe_unused]] const ncjson& aos)
				{
					gcmd<std::string> pro(id, "get_openserver");
					int32_t lopentime = 0;
					if (!m_keyvaluedb.keyvalue(pbdb::db_keyvalue::open_server, lopentime) || lopentime == 0)
					{
						pro.m_data = "fail";
						return;
					}
					pro.m_data = localtime::time2str(lopentime);
				};
		}

		if (handle_cmd::function(loperator, (int32_t)lparm->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
}//namespace ngl