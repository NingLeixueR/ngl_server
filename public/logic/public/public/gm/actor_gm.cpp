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
			, dregister_fun_handle(actor_gm, GM::PROBUFF_GM_GET_NOTICE)
			, dregister_fun_handle(actor_gm, GM::PROBUFF_GM_ADD_NOTICE)
			, dregister_fun_handle(actor_gm, GM::PROBUFF_GM_DEL_NOTICE)
			, dregister_fun_handle(actor_gm, mforward<GM::PROBUFF_GM_GET_NOTICE_RESPONSE>)
			, dregister_fun_handle(actor_gm, mforward<GM::PROBUFF_GM_ADD_NOTICE_RESPONSE>)
			, dregister_fun_handle(actor_gm, mforward<GM::PROBUFF_GM_DEL_NOTICE_RESPONSE>)
		);
	}

}