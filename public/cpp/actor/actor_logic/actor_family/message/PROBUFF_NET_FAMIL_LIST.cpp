#include "actor_family.h"

namespace ngl
{
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata)
	{
		m_family.sync_family(adata.get_data()->identifier(), adata.get_data()->data()->m_familid());
		return true;
	}
}//namespace ngl