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
		auto lparm = adata.get_data();
		auto lpack = adata.m_pack;
		np_actor_reloadcsv pro;
		const auto& lversion = allcsv::all();
		std::ranges::for_each(lversion, [&lparm,&pro](const auto& apair)
			{
				
				if (auto itor = lparm->m_verify.find(apair.first);
					itor != lparm->m_verify.end() && itor->second != apair.second->verify())
				{
					return;
				}
				reload_csv::readcsv(apair.first, pro.m_csvcontent[apair.first]);
			});
		if (pro.m_csvcontent.empty() == false)
		{
			send(lpack->m_id, pro, lpack->m_head.get_request_actor(), id_guid());
		}
		return true;
	}
}//namespace ngl