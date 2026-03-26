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
#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
namespace ngl
{
	bool actor_ranklist::handle([[maybe_unused]] const message<mforward<np_gm>>& adata)
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
			handle_cmd::add("ranklist") = [this](int id, ncjson& aos)
				{
					gcmd<std::string> pro(id, "ranklist");
					struct json_rank
					{
						int32_t m_type = 0;
						int32_t m_page = 0;
						int32_t m_everynum = 0;

						DPROTOCOL(json_rank, m_type, m_page)
					};
					json_rank lrank;
					if (!njson::pop(aos, { "data" }, lrank))
					{
						return true;
					}
					pbnet::PROBUFF_NET_RANKLIST_RESPONSE prorank;
					if (m_ranklist.ranklist_get(nguid::make(), (pbdb::eranklist)lrank.m_type, lrank.m_page, prorank))
					{
						tools::proto2json(prorank, pro.m_data);
					}
					return true;
				};
		}

		if (handle_cmd::function(loperator, (int32_t)lparm->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_ranklist::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_RANKLIST>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		m_ranklist.ranklist_sync(lparm->identifier(), lrecv->mtype(), lrecv->mactivityid(), lrecv->mpage());
		return true;
	}
	bool actor_ranklist::handle([[maybe_unused]] const message<np_activityrank_operator>& adata)
	{
		const auto* lrecv = adata.get_data();
		int32_t lrankid = lrecv->m_rankid;
		m_ranklist.rank_remove(lrankid);
		if (lrecv->m_iscreate)
		{
			m_ranklist.rank_add(lrankid);			
		}
		return true;
	}
	bool actor_ranklist::handle([[maybe_unused]] const message<np_get_rank>& adata)
	{
		const auto* lrecv = adata.get_data();
		auto pro = std::make_shared<np_get_rank_response>();
		pro->m_rankid = lrecv->m_rankid;
		m_ranklist.rank_get(lrecv->m_rankid, pro->m_rolerank);
		return true;
	}
}//namespace ngl