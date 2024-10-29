#include "actor_gatewayg2c.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_gatewayg2c::actor_gatewayg2c() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_G2C,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = nconfig::m_nodeid,
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	actor_gatewayg2c::~actor_gatewayg2c() {}

	void actor_gatewayg2c::nregister()
	{
		register_handle_custom<actor_gatewayg2c>::func<
			np_actor_gatewayinfo_updata
		>(false);
		// Game 2 Client
		nforward::g2c();
	}

	bool actor_gatewayg2c::handle(const message<np_actor_gatewayinfo_updata>& adata)
	{
		auto lpram = adata.get_data();
		std::ranges::for_each(lpram->m_delsocket, [this](i32_socket asocket)
			{
				m_info.remove_socket(asocket);
			});
		std::ranges::for_each(lpram->m_delactorid, [this](i64_actorid aactorid)
			{
				m_info.remove_actorid(aactorid);
			});
		std::ranges::for_each(lpram->m_add, [this](const gateway_socket& agetway)
			{
				m_info.updata(agetway);
			});
		return true;
	}
}//namespace ngl