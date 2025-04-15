#include "actor_csvclient.h"
#include "actor_csvserver.h"
#include "actor_manage.h"

namespace ngl
{
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

	bool actor_csvclient::timer_handle(const message<np_timerparm>&)
	{
		log_error()->print("############actor_csvclient::timer_handle###########");
		auto pro = std::make_shared<np_actor_csv_verify_version>();
		allcsv::foreach_verify(pro->m_verify);
		send_actor(actor_csvserver::actorid(), pro);
		return true;
	}
}//namespace ngl