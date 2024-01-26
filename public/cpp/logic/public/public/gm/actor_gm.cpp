#include "actor_gm.h"


namespace ngl
{
	actor_gm::actor_gm() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GM,
					.m_area = ttab_servers::tab()->m_area,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_gm::actor_register()
	{
		// Ð­Òé×¢²á
		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_gm>(false
			, (Tfun<actor_gm, GM::PROBUFF_GM_GET_NOTICE>) & actor_gm::handle<ACTOR_NOTICE>
			, (Tfun<actor_gm, GM::PROBUFF_GM_ADD_NOTICE>) & actor_gm::handle<ACTOR_NOTICE>
			, (Tfun<actor_gm, GM::PROBUFF_GM_DEL_NOTICE>) & actor_gm::handle<ACTOR_NOTICE>
			, (Tfun<actor_gm, mforward<GM::PROBUFF_GM_GET_NOTICE_RESPONSE>>) & actor_gm::handle
			, (Tfun<actor_gm, mforward<GM::PROBUFF_GM_ADD_NOTICE_RESPONSE>>) & actor_gm::handle
			, (Tfun<actor_gm, mforward<GM::PROBUFF_GM_DEL_NOTICE_RESPONSE>>) & actor_gm::handle
			, (Tfun<actor_gm, GM::PROBUFF_GM_RECHARGE>) & actor_gm::handle_role
			, (Tfun<actor_gm, mforward<GM::PROBUFF_GM_RECHARGE_RESPONSE>>) & actor_gm::handle
		);
	}

}