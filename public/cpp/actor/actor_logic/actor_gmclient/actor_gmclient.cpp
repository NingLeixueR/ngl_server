#include "actor_gmclient.h"

namespace ngl
{
	actor_gmclient::actor_gmclient() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GMCLIENT,
					.m_area = tab_self_area,
					.m_id = ttab_servers::tab()->m_id,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_gmclient::actor_type()
	{
		return ACTOR_GMCLIENT;
	}

	i64_actorid actor_gmclient::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, ttab_servers::tab()->m_id);
	}

	void actor_gmclient::init() 
	{
	}

	void actor_gmclient::loaddb_finish(bool adbishave) 
	{
	}

	void actor_gmclient::get_allprotocol(protocols& apro)
	{
		tprotocol::get_allprotocol(apro.m_promap, apro.m_custommap);
	}

	void actor_gmclient::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_gmclient>::func<
			mforward<np_gm>
		>(false);
	}
}//namespace ngl