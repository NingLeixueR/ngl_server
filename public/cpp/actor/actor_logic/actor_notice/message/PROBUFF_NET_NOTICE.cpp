#include "actor_notice.h"

namespace ngl
{
	bool actor_notice::handle(const message<mforward<pbnet::PROBUFF_NET_NOTICE>>& adata)
	{
		auto pro = m_notice.sync_notice(-1);
		send_client(adata.get_data()->identifier(), pro);
		return true;
	}
}//namespace ngl