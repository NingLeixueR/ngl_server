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
#include "actor_ranklist.h"
#include "actor_brief.h"
namespace ngl
{
	bool actor_ranklist::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::njson_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::read(lojson, "operator", loperator))
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("ranklist") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<std::string> pro(id, "ranklist");
					struct json_rank
					{
						int32_t m_type = 0;
						int32_t m_page = 0;
						int32_t m_everynum = 0;

						dprotocol(json_rank, m_type, m_page)
					};
					json_rank lrank;
					if (!njson::read(aos, "data", lrank))
					{
						return true;
					}
					pbnet::PROBUFF_NET_RANKLIST_RESPONSE prorank;
					if (m_ranklist.get_ranklist(nguid::make(), (pbdb::eranklist)lrank.m_type, lrank.m_page, prorank))
					{
						tools::proto2json(prorank, pro.m_data);
					}
					return true;
				};
		}

		if (handle_cmd::function(loperator, (int32_t)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_ranklist::handle(const message<mforward<pbnet::PROBUFF_NET_RANKLIST>>& adata)
	{
		auto lrecv = adata.get_data()->data();
		m_ranklist.sync_ranklist(adata.get_data()->identifier(), lrecv->mtype(), lrecv->mactivityid(), lrecv->mpage());
		return true;
	}
	bool actor_ranklist::handle(const message<np_activityrank_operator>& adata)
	{
		auto lrecv = adata.get_data();
		int32_t lrankid = lrecv->m_rankid;
		m_ranklist.remove_rank(lrankid);
		if (lrecv->m_iscreate)
		{
			m_ranklist.add_rank(lrankid);			
		}
		return true;
	}
	bool actor_ranklist::handle(const message<np_get_rank>& adata)
	{
		auto lrecv = adata.get_data();
		auto pro = std::make_shared<np_get_rank_response>();
		pro->m_rankid = lrecv->m_rankid;
		m_ranklist.get_rank(lrecv->m_rankid, pro->m_rolerank);
		return true;
	}
}//namespace ngl
