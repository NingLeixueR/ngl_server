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
}//namespace ngl
