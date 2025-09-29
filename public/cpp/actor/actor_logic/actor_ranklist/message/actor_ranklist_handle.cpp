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
						int32_t m_type;
						int32_t m_page;
						int32_t m_everynum;

						dprotocol(json_rank, m_type, m_page)
					};
					json_rank lrank;
					if (!njson::read(aos, "data", lrank))
					{
						return true;
					}
					auto prorank = m_ranklist.get_ranklist(nguid::make(), (pbdb::eranklist)lrank.m_type, lrank.m_page);
					tools::proto2json(*prorank, pro.m_data);
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
		std::map<i64_actorid, pbdb::db_brief>& lmap = tdb_brief::nsp_cwrite<actor_ranklist>::instance(id_guid()).get_map();
		for (std::pair<const i64_actorid, pbdb::db_brief>& item : lmap)
		{
			item.second.mutable_mactivityvalues()->mutable_mactivity_rolelv()->erase(lrankid);
			item.second.mutable_mactivityvalues()->mutable_mactivity_rolegold()->erase(lrankid);
		}
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
