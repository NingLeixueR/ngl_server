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
					.m_area = ttab_servers::tab()->m_area,
					.m_id = ttab_servers::tab()->m_id,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_gmclient::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_gmclient>::func<
			mforward<np_gm>
		>(false);
	}

	void actor_gmclient::get_allprotocol(protocols& apro)
	{
		tprotocol::get_allprotocol(apro.m_promap, apro.m_custommap);
	}

	bool actor_gmclient::handle(const message<mforward<np_gm>>& adata)
	{
		gcmd<protocols> lpro;
		get_allprotocol(lpro.m_data);
		lpro.id = adata.get_data()->identifier();
		lpro.m_operator = "all_protocol";
		return true;
	}
}