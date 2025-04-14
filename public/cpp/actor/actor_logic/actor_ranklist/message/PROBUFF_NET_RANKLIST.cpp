#include "actor_ranklist.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_ranklist::handle(const message<mforward<pbnet::PROBUFF_NET_RANKLIST>>& adata)
	{
		auto lrecv = adata.get_data()->data();
		m_ranklist.sync_ranklist(adata.get_data()->identifier(), lrecv->m_type(), lrecv->m_page(), lrecv->m_everynum());
		return true;
	}
}//namespace ngl