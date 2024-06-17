#include "actor_brief.h"
#include "net.pb.h"

namespace ngl
{
	actor_brief::actor_brief() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_BRIEF,
						.m_area = ttab_servers::tab()->m_area
					},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_brief::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_brief>::func<
			np_actor_roleinfo
		>(false);
	}

	bool actor_brief::handle(message<np_actor_roleinfo>& adata)
	{
		m_briefdb.update(*adata.get_data()->m_vecinfo.m_data.get());

		std::shared_ptr<np_actor_roleinfo>& lparm = adata.get_shared_data();
		if (lparm == nullptr)
			return true;
		actor::static_send_actor(nguid::make_self(ACTOR_NOTICE), nguid::make(), lparm);
		actor::static_send_actor(nguid::make_self(ACTOR_CHAT), nguid::make(), lparm);

		actor::static_send_actor(
			nguid::make(
				ACTOR_CHAT,
				ttab_servers::tab()->m_crossarea,
				nguid::none_actordataid()
			),
			nguid::make(),
			lparm
		);
		return true;
	}
}//namespace ngl