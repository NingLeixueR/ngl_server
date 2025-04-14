#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_robot::actor_robot(i16_area aarea, i32_actordataid arobotid, void*) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROBOT,
					.m_area = aarea,
					.m_id = arobotid,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
			})
	{
		assert(aarea == tab_self_area);
	}

	const char* actor_robot::kcp_session()
	{
		return m_kcpsessionmd5.c_str();
	}

	void actor_robot::nregister()
	{
		nforward::g2c();

		register_handle_proto<actor_robot>::func<
			pbnet::PROBUFF_NET_KCPSESSION_RESPONSE
		>(false);
	}
}//namespace ngl