#include "actor_notice.h"


namespace ngl
{

	actor_notice::actor_notice() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_NOTICE,
					.m_area = ttab_servers::tab()->m_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	void actor_notice::actor_register()
	{
		// Ð­Òé×¢²á
		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_notice>(
			true
			, null<mforward<GM::PROBUFF_GM_GET_NOTICE>>
			, null<mforward<GM::PROBUFF_GM_ADD_NOTICE>>
			, null<mforward<GM::PROBUFF_GM_DEL_NOTICE>>
		);
	}

}