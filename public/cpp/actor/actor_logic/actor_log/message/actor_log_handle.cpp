#include "actor_log.h"

namespace ngl
{
	bool actor_log::handle(const message<np_logitem>& adata)
	{
		const np_logitem* ldata = adata.get_data();
		m_log->printf(ldata);
		return true;
	}
}//namespace ngl