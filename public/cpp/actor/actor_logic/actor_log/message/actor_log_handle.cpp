#include "actor_log.h"
namespace ngl
{
	bool actor_log::handle(const message<np_logitem>& adata)
	{
		const np_logitem* ldata = adata.get_data();
		auto itor = m_logarr.find(ldata->m_type);
		if (itor == m_logarr.end())
		{
			return true;
		}
		itor->second->printf(ldata);
		return true;
	}
}//namespace ngl
