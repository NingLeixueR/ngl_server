#include "actor_gateway_g2c.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_gateway_g2c::handle(const message<np_actor_gatewayinfo_updata>& adata)
	{
		m_info.updata(*adata.get_data());
		return true;
	}
}//namespace ngl