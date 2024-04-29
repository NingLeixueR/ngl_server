#include "actor_csvclient.h"
#include "actor_csvserver.h"
#include "actor_manage.h"

namespace ngl
{
	actor_csvclient::actor_csvclient() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVCLIENT,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = nconfig::m_nodeid
				},
				.m_weight = 0x7fffffff,
			})
	{}

	void actor_csvclient::init()
	{
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, 60) == false)
		{
			LogLocalError("actor_csvclient::init() make_timerparm::make_interval(tparm, 60) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	void actor_csvclient::nregister()
	{
		actor_csvclient::register_timer<actor_csvclient>(&actor_csvclient::timer_handle);
		
		type_register_actor_handle<EPROTOCOL_TYPE_CUSTOM, actor_csvclient>::func<
			np_actor_reloadcsv
		>(true);
	}

	actor_csvclient::~actor_csvclient()
	{
	}

	bool actor_csvclient::handle(message<np_actor_reloadcsv>& adata)
	{
		for (auto& [key, value] : adata.m_data->m_csvcontent)
		{
			reload_csv::save(key, value);
		}
		actor_suspendthread ltemp;
		for (auto& [key, value] : adata.m_data->m_csvcontent)
		{
			reload_csv::reload(key);
		}
		return true;
	}

	bool actor_csvclient::timer_handle(message<timerparm>& adata)
	{
		LogLocalError("############actor_csvclient::timer_handle###########");
		auto pro = std::make_shared<np_actor_csv_verify_version>();
		allcsv::foreach_version(pro->m_version);
		send_actor(actor_csvserver::actorid(), pro);
		return true;
	}
}//namespace ngl