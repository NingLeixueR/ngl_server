#include "actor_gateway_c2g.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_gateway_c2g::handle(const message<np_actor_gatewayinfo_updata>& adata)
	{
		m_info.updata(*adata.get_data());
		return true;
	}
}//namespace ngl