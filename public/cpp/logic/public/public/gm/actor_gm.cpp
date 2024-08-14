#include "actor_gm.h"

namespace ngl
{
	actor_gm::actor_gm() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GM,
					.m_area = ttab_servers::tab()->m_area,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_gm::nregister()
	{
		// 协议注册
		register_handle_custom<actor_gm>::func<
			ngl::np_gm
			, mforward<ngl::np_gm_response>
		>(false);
	}

	void actor_gm::sendbytype(ENUM_ACTOR atype, const pack* apack, ngl::np_gm& apro)
	{
		auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
		send_actor(atype, pro, true);
	}

	void actor_gm::sendbyactorid(i64_actorid aactorid, const pack* apack, ngl::np_gm& apro)
	{
		auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
		send_actor(aactorid, pro);
	}

	bool actor_gm::reply_php(const pack* apack, ngl::np_gm_response& adata)const
	{
		send(apack->m_id, adata, nguid::make(), nguid::make());
		return true;
	}

	bool actor_gm::handle(message<ngl::np_gm>& adata)
	{
		log_error()->print("php2gm [{}]", adata.get_data()->m_json);
		ngl::json_read lreadjson(adata.get_data()->m_json.c_str());

		if (std::string lactorname; lreadjson.read("actor_name", lactorname))
		{// ### 单例
			if (lactorname == "ACTOR_GM")
			{
				std::string loperator;
				if (lreadjson.read("operator", loperator) == false)
				{
					return true;
				}
				if (handle_cmd::empty())
				{
					struct gm_guid
					{
						std::string m_actor_name;
						int16_t m_area;
						int32_t m_dataid;

						jsonfunc("actor_name", m_actor_name, "area", m_area, "dataid", m_dataid)
					};

					handle_cmd::push("guid", [this,&adata](ngl::json_read& aos)
						{
							gm_guid lguid;
							if (aos.read("data", lguid))
							{
								ENUM_ACTOR ltype;
								ltype = em<ENUM_ACTOR>::get_enum(lguid.m_actor_name.c_str());
								if (ltype == em<ENUM_ACTOR>::enum_null())
									return;
								ngl::json_write lwritejson;
								lwritejson.write("guid", nguid::make(ltype, lguid.m_area, lguid.m_dataid));
								ngl::np_gm_response lresponse;
								lwritejson.get(lresponse.m_json);
								reply_php(adata.m_pack, lresponse);
								return;
							}
						}
					);

					handle_cmd::push("close_actor", [this, &adata](ngl::json_read& aos)
						{
							gm_guid lguid;
							if (aos.read("data", lguid))
							{
								ENUM_ACTOR ltype;
								ltype = em<ENUM_ACTOR>::get_enum(lguid.m_actor_name.c_str());
								if (ltype == em<ENUM_ACTOR>::enum_null())
									return;
								nguid::make(ltype, lguid.m_area, lguid.m_dataid);
								auto pro = std::make_shared<np_actor_close>();
								send_actor(nguid::make(ltype, lguid.m_area, lguid.m_dataid), pro);
								return;
							}
						});
				}

				if (handle_cmd::function(loperator, lreadjson) == false)
				{
					log_error()->print("GM actor_gm operator[{}] ERROR", loperator);
				}
				return true;
			}

			ENUM_ACTOR ltype;
			ltype = em<ENUM_ACTOR>::get_enum(lactorname.c_str());
			if (ltype == em<ENUM_ACTOR>::enum_null())
			{
				if (lactorname == "ACTOR_DB")
				{
					int32_t ltype = 0;
					if (lreadjson.read("db", ltype) == false)
					{
						return true;
					}
					i64_actorid lactorid = nguid::make(
						db_enum((pbdb::ENUM_DB)(ltype)), ttab_servers::tab()->m_area, nguid::none_actordataid()
					);
					sendbyactorid(lactorid, adata.m_pack, *adata.get_data());
				}
				return true;
			}
			
			sendbytype(ltype, adata.m_pack, *adata.get_data());
			return true;
		}
		else if (i64_actorid lactorid; lreadjson.read("actor_id", lactorid))
		{// ### 非单例
			sendbyactorid(lactorid, adata.m_pack, *adata.get_data());
			return true;
		}

		return true;
	}

	bool actor_gm::handle(message<mforward<ngl::np_gm_response>>& adata)
	{
		log_error()->print("gm2php [{}]", adata.get_data()->data()->m_json);
		send((i32_sessionid)adata.get_data()->identifier(), *adata.get_data()->data(), nguid::make(), nguid::make());
		return true;
	}
}// namespace ngl