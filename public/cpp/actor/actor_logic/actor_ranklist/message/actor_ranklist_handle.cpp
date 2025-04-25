#include "actor_ranklist.h"
#include "actor_brief.h"
namespace ngl
{
	bool actor_ranklist::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::push("ranklist", [this](int id, const ngl::json_read& aos)
				{
					gcmd<std::string> pro(id, "ranklist");
					struct json_rank
					{
						int32_t m_type;
						int32_t m_page;
						int32_t m_everynum;

						jsonfunc("type", m_type, "page", m_page, "everynum", m_everynum)
					};
					json_rank lrank;
					if (aos.read("data", lrank) == false)
					{
						return;
					}
					auto prorank = m_ranklist.get_ranklist((pbdb::eranklist)lrank.m_type, lrank.m_page, lrank.m_everynum);
					if (tools::proto2json<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>(*prorank, pro.m_data) == false)
					{
						return;
					}
				}
			);
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
		m_ranklist.sync_ranklist(adata.get_data()->identifier(), lrecv->m_type(), lrecv->m_page(), lrecv->m_everynum());
		return true;
	}
}//namespace ngl
