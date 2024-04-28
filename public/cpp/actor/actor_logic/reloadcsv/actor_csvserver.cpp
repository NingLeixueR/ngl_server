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

	void actor_csvserver::init() {}

	void actor_csvserver::nregister()
	{
		register_actor_handle<EPROTOCOL_TYPE_CUSTOM
			, actor_csvserver
			, np_actor_csv_verify_version
		>(true);
	}

	actor_csvserver::~actor_csvserver()
	{
	}

	bool actor_csvserver::handle(message<np_actor_csv_verify_version>& adata)
	{
		LogLocalError("############actor_reloadcsv_distribute::handle###########");
		auto lparm = adata.m_data;
		auto lpack = adata.m_pack;
		np_actor_reloadcsv pro;
		std::map<std::string, csvbase*>& lversion = allcsv::all();
		for (auto [key, value] : lversion)
		{
			auto itor = lparm->m_version.find(key);
			if (itor != lparm->m_version.end() && itor->second >= value->version())
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