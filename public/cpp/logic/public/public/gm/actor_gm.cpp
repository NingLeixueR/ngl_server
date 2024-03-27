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
			//, (Tfun<actor_gm, GM::PROBUFF_GM_GET_NOTICE>) & actor_gm::handle<ACTOR_NOTICE>
			//, (Tfun<actor_gm, GM::PROBUFF_GM_ADD_NOTICE>) & actor_gm::handle<ACTOR_NOTICE>
			//, (Tfun<actor_gm, GM::PROBUFF_GM_DEL_NOTICE>) & actor_gm::handle<ACTOR_NOTICE>
			//, (Tfun<actor_gm, mforward<GM::PROBUFF_GM_GET_NOTICE_RESPONSE>>) & actor_gm::handle
			//, (Tfun<actor_gm, mforward<GM::PROBUFF_GM_ADD_NOTICE_RESPONSE>>) & actor_gm::handle
			//, (Tfun<actor_gm, mforward<GM::PROBUFF_GM_DEL_NOTICE_RESPONSE>>) & actor_gm::handle
			//, (Tfun<actor_gm, GM::PROBUFF_GM_RECHARGE>) & actor_gm::handle_role
			//, (Tfun<actor_gm, mforward<GM::PROBUFF_GM_RECHARGE_RESPONSE>>) & actor_gm::handle
		);
	}

	bool actor_gm::handle(message<ngl::np_gm>& adata)
	{
		ngl::ojson lreadjson(adata.m_data->m_json.c_str());

		std::string lactorname;
		if (lreadjson.read("actor_name", lactorname) == false)
		{
			return true;
		}

		if (lactorname == "ACTOR_GM")
		{
			return true;
		}

		// ### 单例
		static std::map<std::string, ENUM_ACTOR> lsignlocal
		{
			{"ACTOR_NOTICE", ACTOR_NOTICE},
		};
		auto itor = lsignlocal.find(lactorname);
		if (itor != lsignlocal.end())
		{
			sendsign(itor->second, adata.m_pack, *adata.m_data);
			return true;
		}
		

		// ### 非单例
		i64_actorid lactorid;
		if (lreadjson.read("actor_id", lactorid) == false)
		{
			return true;
		}

		sendnosign(lactorid, adata.m_pack, *adata.m_data);

		return true;
	}

	bool actor_gm::handle(message<mforward<ngl::np_gm_response>>& adata)
	{
		std::cout << adata.m_data->data()->m_json << std::endl;
		send(adata.m_data->identifier(), *adata.m_data->data(), nguid::make(), nguid::make());
		return true;
	}

}// namespace ngl