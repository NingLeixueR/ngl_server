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

	void actor_notice::nregister()
	{
		// Ð­Òé×¢²á
		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_notice>(
			true
			, dregister_fun_handle(actor_notice, mforward<GM::PROBUFF_GM_GET_NOTICE>)
			, dregister_fun_handle(actor_notice, mforward<GM::PROBUFF_GM_ADD_NOTICE>)
			, dregister_fun_handle(actor_notice, mforward<GM::PROBUFF_GM_DEL_NOTICE>)
		);
	}
}