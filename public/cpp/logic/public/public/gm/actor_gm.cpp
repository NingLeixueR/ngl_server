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
		// Э��ע��
		register_handle_custom<actor_gm>::func<
			ngl::np_gm
			, mforward<ngl::np_gm_response>
		>(false);
	}

	void actor_gm::sendbytype(ENUM_ACTOR atype, const pack* apack, const ngl::np_gm& apro)
	{
		auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
		send_actor(atype, pro, true);
	}

	void actor_gm::sendbyactorid(i64_actorid aactorid, const pack* apack, const ngl::np_gm& apro)
	{
		auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
		send_actor(aactorid, pro);
	}

	bool actor_gm::reply_php(const pack* apack, ngl::np_gm_response& adata)const
	{
		send(apack->m_id, adata, nguid::make(), nguid::make());
		return true;
	}

	bool actor_gm::handle(const message<ngl::np_gm>& adata)
	{
		log_error()->print("php2gm [{}]", adata.get_data()->m_json);
		ngl::json_read lreadjson(adata.get_data()->m_json.c_str());

		std::string lactorname;
		i64_actorid lactorid = -1;
		if (lreadjson.read("actor_name", lactorname))
		{// ### ����
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

					handle_cmd::push("guid", [this](const json_read& aos, const message<ngl::np_gm>* adata)
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
								reply_php(adata->m_pack, lresponse);
								return;
							}
						}
					);

					handle_cmd::push("all_protocol", [this](const json_read& aos, const message<ngl::np_gm>* adata)
						{
							ngl::json_write lwritejson;
							struct aprotocols
							{
								std::map<int, std::string> m_promap;
								std::map<int, std::string> m_custommap;

								jsonfunc("proto", m_promap, "custom", m_custommap)
							};
							aprotocols pro;
							tprotocol::get_allprotocol(pro.m_promap, pro.m_custommap);
							json_write ljson;
							ljson.write("all_protocol", pro);
							ngl::np_gm_response lresponse;
							ljson.get(lresponse.m_json);
							reply_php(adata->m_pack, lresponse);

							return;
						}
					);


					handle_cmd::push("close_actor", [this](const json_read& aos, const message<ngl::np_gm>* adata)
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

				if (handle_cmd::function(loperator, lreadjson, &adata) == false)
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
					i64_actorid ldbactorid = nguid::make(
						db_enum((pbdb::ENUM_DB)(ltype)), ttab_servers::tab()->m_area, nguid::none_actordataid()
					);
					sendbyactorid(ldbactorid, adata.m_pack, *adata.get_data());
				}
				return true;
			}
			
			sendbytype(ltype, adata.m_pack, *adata.get_data());
			return true;
		}
		else if (lreadjson.read("actor_id", lactorid))
		{// ### �ǵ���
			sendbyactorid(lactorid, adata.m_pack, *adata.get_data());
			return true;
		}

		return true;
	}

	bool actor_gm::handle(const message<mforward<ngl::np_gm_response>>& adata)
	{
		log_error()->print("gm2php [{}]", adata.get_data()->data()->m_json);
		send((i32_sessionid)adata.get_data()->identifier(), *adata.get_data()->data(), nguid::make(), nguid::make());
		return true;
	}
}// namespace ngl