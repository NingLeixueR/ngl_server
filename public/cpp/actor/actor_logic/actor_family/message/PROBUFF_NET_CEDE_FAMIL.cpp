#include "actor_family.h"

namespace ngl
{
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_CEDE_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lcederoleid = adata.get_data()->data()->m_roleid();

		return true;
	}
}//namespace ngl