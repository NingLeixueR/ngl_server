#include "actor_mail.h"

namespace ngl
{
	actor_mail::actor_mail() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MAIL,
					.m_area = ttab_servers::tab()->m_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	void actor_mail::nregister()
	{
		// Ð­Òé×¢²á
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_mail>(
			true
			, dregister_fun_handle(actor_mail, np_actor_addmail)
			);
	}
}// namespace ngl