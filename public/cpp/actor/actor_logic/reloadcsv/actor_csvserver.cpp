#include "actor_csvserver.h"


namespace ngl
{
	actor_csvserver::actor_csvserver() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVSERVER,
				},
				.m_weight = 0x7fffffff,
			})
	{}

	void actor_csvserver::init() 
	{
	}

	i64_actorid actor_csvserver::actorid()
	{
		return nguid::make(ACTOR_CSVSERVER, nguid::none_area(), nguid::none_actordataid());
	}

	void actor_csvserver::nregister()
	{
		register_handle_custom<actor_csvserver>::func<
			np_actor_csv_verify_version
		>(true);
	}

	actor_csvserver::~actor_csvserver()
	{
	}

	bool actor_csvserver::handle(message<np_actor_csv_verify_version>& adata)
	{
		log_error()->print("############actor_reloadcsv_distribute::handle###########");
		auto lparm = adata.m_data;
		auto lpack = adata.m_pack;
		np_actor_reloadcsv pro;
		std::map<std::string, csvbase*>& lversion = allcsv::all();
		for (const auto [key, value] : lversion)
		{
			auto itor = lparm->m_verify.find(key);
			if (itor != lparm->m_verify.end() && itor->second != value->verify())
				continue;
			reload_csv::readcsv(key, pro.m_csvcontent[key]);
		}
		if (pro.m_csvcontent.empty() == false)
		{
			send(lpack->m_id, pro, lpack->m_head.get_request_actor(), id_guid());
		}
		return true;
	}
}//namespace ngl