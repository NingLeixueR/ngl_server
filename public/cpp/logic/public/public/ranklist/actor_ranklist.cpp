#include "actor_ranklist.h"
#include "actor_brief.h"

namespace ngl
{
	actor_ranklist::actor_ranklist() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_RANKLIST,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		std::set<i64_actorid> ldataid;
		tdb_brief::nsp_cli<actor_ranklist>::init(actor_brief::actor_type(), this, ldataid);
	}

	void actor_ranklist::nregister()
	{
		register_handle_custom<actor_ranklist>::func<
			mforward<np_gm>
		>(true);

		register_handle_proto<actor_ranklist>::func<
			mforward<pbnet::PROBUFF_NET_RANKLIST>
		>(true);

	}

	bool actor_ranklist::handle(const message<mforward<pbnet::PROBUFF_NET_RANKLIST>>& adata)
	{
		auto lrecv = adata.get_data()->data();
		m_ranklist.sync_ranklist(adata.get_data()->identifier(), lrecv->m_type(), lrecv->m_page(), lrecv->m_everynum());
		return true;
	}

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
					gcmd<std::string> pro(id);
					auto prorank = m_ranklist.get_ranklist((pbdb::eranklist)lrank.m_type, lrank.m_page, lrank.m_everynum);
					if (tools::protojson<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>(*prorank, pro.m_data) == false)
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
}// namespace ngl