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
					.m_area = tab_self_area,
					.m_id	= nconfig::m_nodeid
				},
				.m_weight = 0x7fffffff,
			})
	{}

	void actor_csvclient::init()
	{
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, 5*60) == false)
		{
			log_error()->print("actor_csvclient::init() make_timerparm::make_interval(tparm, 60) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	void actor_csvclient::nregister()
	{
		actor_csvclient::register_timer<actor_csvclient>(&actor_csvclient::timer_handle);
		
		register_handle_custom<actor_csvclient>::func<
			np_actor_reloadcsv
		>(true);
	}

	actor_csvclient::~actor_csvclient()
	{
	}

	bool actor_csvclient::handle(const message<np_actor_reloadcsv>& adata)
	{
		for (const auto& [key, value] : adata.get_data()->m_csvcontent)
		{
			reload_csv::save(key, value);
		}
		actor_suspendthread ltemp;
		for (const auto& [key, value] : adata.get_data()->m_csvcontent)
		{
			reload_csv::reload(key);
		}
		return true;
	}

	bool actor_csvclient::timer_handle(const message<timerparm>&)
	{
		log_error()->print("############actor_csvclient::timer_handle###########");
		auto pro = std::make_shared<np_actor_csv_verify_version>();
		allcsv::foreach_verify(pro->m_verify);
		send_actor(actor_csvserver::actorid(), pro);
		return true;
	}
}//namespace ngl