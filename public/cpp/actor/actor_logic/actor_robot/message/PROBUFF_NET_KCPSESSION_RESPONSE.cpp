#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata)
	{
		const tab_servers* tab = ttab_servers::tab();
		m_kcpsessionmd5 = adata.get_data()->m_kcpsession();

		const tab_servers* tabgame = ttab_servers::tab("game", tab->m_area, 1);
		net_works const* lpworks = ttab_servers::nworks(ENET_KCP, tabgame);
		if (lpworks == nullptr)
			return false;
		return connect_kcp(m_kcp, lpworks->m_ip, lpworks->m_port);
	}
}//namespace ngl