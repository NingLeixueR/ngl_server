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
					.m_id = ttab_servers::instance().tab()->m_id,
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
		return nguid::make(actor_type(), area, ttab_servers::instance().tab()->m_id);
	}

	void actor_gmclient::init()
	{
		// ��DB�ṹ:DB.set(this);

		// ����timer_handle��ʱ��
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gmclient::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
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
		// ��ʱ��
		actor::register_timer<actor_gmclient>(&actor_gmclient::timer_handle);

		// ���Զ���np_��Ϣ
		register_handle_custom<actor_gmclient>::func<
			mforward<np_gm>
		>(true);

		// ��pb��Ϣ
		register_handle_proto<actor_gmclient>::func<
		>(true);
	}

	bool actor_gmclient::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_gmclient::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl