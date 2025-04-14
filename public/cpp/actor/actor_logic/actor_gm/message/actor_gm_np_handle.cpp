#include "actor_gmclient.h"
#include "actor_gm.h"

namespace ngl
{
	bool actor_gm::handle(const message<ngl::np_gm>& adata)
	{
		log_error()->print("php2gm [{}]", adata.get_data()->m_json);
		ngl::json_read lreadjson(adata.get_data()->m_json.c_str());

		std::string lactorname;
		i64_actorid lactorid = -1;
		if (lreadjson.read("actor_name", lactorname))
		{
			// ### µ¥Àý
			if (lactorname == "ACTOR_GM")
			{
				std::string loperator;
				if (lreadjson.read("operator", loperator) == false)
				{
					return true;
				}
				init_handle_cmd();
				if (distribute_gmclient::getInstance().distribute(loperator, lreadjson, &adata, this) == false)
				{
					return true;
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
					i64_actorid ldbactorid = nguid::make(db_enum((pbdb::ENUM_DB)(ltype)), tab_self_area, nguid::none_actordataid());
					sendbyactorid(ldbactorid, adata.m_pack, *adata.get_data());
				}
				return true;
			}

			sendbytype(ltype, adata.m_pack, *adata.get_data());
			return true;
		}
		else if (lreadjson.read("actor_id", lactorid))
		{// ### ·Çµ¥Àý
			sendbyactorid(lactorid, adata.m_pack, *adata.get_data());
			return true;
		}

		return true;
	}

	bool actor_gm::handle(const message<mforward<ngl::np_gm_response>>& adata)
	{
		return sendtophp((i32_sessionid)adata.get_data()->identifier(), *adata.get_data()->data());
	}
}//namespace ngl