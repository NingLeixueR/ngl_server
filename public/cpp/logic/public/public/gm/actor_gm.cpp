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
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_gm>(false
			, dregister_fun_handle(actor_gm, ngl::np_gm)
			, dregister_fun_handle(actor_gm, mforward<ngl::np_gm_response>)
		);
	}

	bool actor_gm::handle(message<ngl::np_gm>& adata)
	{
		ngl::ojson lreadjson(adata.m_data->m_json.c_str());

		// ### 单例
		std::string lactorname;
		// ### 非单例
		i64_actorid lactorid;
		if (lreadjson.read("actor_name", lactorname))
		{
			if (lactorname == "ACTOR_GM")
			{
				std::string loperator;
				if (lreadjson.read("operator", loperator) == false)
				{
					return true;
				}
				if (loperator == "guid")
				{
					struct gm_guid
					{
						std::string m_actor_name;
						int16_t m_area;
						int32_t m_dataid;

						jsonfunc("actor_name", m_actor_name, "area", m_area, "dataid", m_dataid)
					};
					gm_guid lguid;
					if (lreadjson.read("data", lguid))
					{
						ENUM_ACTOR ltype;
						if (nactortype::getInstance().name2enum(lguid.m_actor_name, ltype) == false)
							return true;
						ngl::ijson lwritejson;
						lwritejson.write("guid", nguid::make(ltype, lguid.m_area, lguid.m_dataid));
						ngl::np_gm_response lresponse;
						lwritejson.get(lresponse.m_json);
						ret_gm(adata.m_pack, lresponse);
						
						return true;
					}
				}

				return true;
			}

			ENUM_ACTOR ltype;
			if (nactortype::getInstance().name2enum(lactorname, ltype) == false)
			{
				if (lactorname == "ACTOR_DB")
				{
					int32_t ltype = 0;
					if (lreadjson.read("db", ltype))
					{
						lactorid = nguid::make(
							db_enum(EPROTOCOL_TYPE::EPROTOCOL_TYPE_PROTOCOLBUFF, (pbdb::ENUM_DB)(ltype)),
							ttab_servers::tab()->m_area,
							nguid::none_actordataid()
						);
						sendnosign(lactorid, adata.m_pack, *adata.m_data);
					}
				}
				return true;
			}
			
			sendsign(ltype, adata.m_pack, *adata.m_data);
			return true;
		}
		else if (lreadjson.read("actor_id", lactorid))
		{
			sendnosign(lactorid, adata.m_pack, *adata.m_data);
			return true;
		}

		return true;
	}

	bool actor_gm::handle(message<mforward<ngl::np_gm_response>>& adata)
	{
		std::cout << adata.m_data->data()->m_json << std::endl;
		send(adata.m_data->identifier(), *adata.m_data->data(), nguid::make(), nguid::make());
		return true;
	}

}// namespace ngl