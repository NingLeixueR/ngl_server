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

	}

	bool actor_ranklist::handle(const message<mforward<pbnet::PROBUFF_NET_RANKLIST>>& adata)
	{
		m_ranklist.sync_ranklist(adata.get_data()->identifier(), adata.get_data()->data()->m_type());
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
					int32_t lenum;
					if (aos.read("data", lenum) == false)
						return;
					gcmd<std::string> pro;
					auto prorank = m_ranklist.get_ranklist((pbdb::eranklist)lenum);
					if (tools::protojson<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>(*prorank, pro.m_data) == false)
					{
						return;
					}
				}
			);
		}

		if (handle_cmd::function(loperator, adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
}// namespace ngl