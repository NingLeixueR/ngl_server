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
// File overview: Message handler registration and dispatch for the gmclient actor.
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
namespace ngl
{
	bool actor_gmclient::handle([[maybe_unused]] const message<mforward<np_gm>>& adata)
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
			handle_cmd::add("all_protocol") = [this](int id, [[maybe_unused]] ncjson& aos)
				{
					gcmd<protocols> lpro(id, "all_protocol");
					allprotocol(lpro.m_data);
				};

			handle_cmd::add("server_stat") = [this](int id, [[maybe_unused]] ncjson& aos)
				{
					gcmd<msg_actor_stat> lpro(id, "server_stat");
					actor_manage::instance().get_actor_stat(lpro.m_data);
				};

			handle_cmd::add("set_time") = [this](int id, ncjson& aos)
				{
					gcmd<std::string> lpro(id, "set_time");
					struct operator_set_time
					{
						int32_t m_time = 0;
						DPROTOCOL(operator_set_time, m_time)
					};
					operator_set_time ltime;
					if (njson::pop(aos, { "data" }, ltime))
					{
						bool lbool = tools::time::settime(ltime.m_time);
						lpro.m_data = std::format("set time {} # {}", tools::time::time2str("%Y-%m-%d %H:%M:%S"), lbool ? "success" : "fail");
					}
				};

				handle_cmd::add("get_time") = [this](int id, [[maybe_unused]] ncjson& aos)
				{
					gcmd<std::string> lpro(id, "get_time", tools::time::time2str("%Y-%m-%d %H:%M:%S"));
				};
		}

		if (handle_cmd::function(loperator, (int32_t)lparm->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_gmclient operator[{}] ERROR", loperator);
		}
		return true;
	}
}//namespace ngl
