#include "actor_brief.h"
#include "net.pb.h"
#include "nsp_server.h"

namespace ngl
{
	actor_brief::actor_brief() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_BRIEF,
						.m_area = tab_self_area,
						.m_manage_dbclient = true
					},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		tdb_brief::nsp_ser::init(&m_briefdb);
	}

	ENUM_ACTOR actor_brief::actor_type()
	{
		return ACTOR_BRIEF;
	}

	i64_actorid actor_brief::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_brief::init()
	{
		// ��DB�ṹ:DB.set(this);
		m_briefdb.set(this);

		// ����timer_handle��ʱ��
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_brief::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_brief::loaddb_finish(bool adbishave)
	{
	}

	void actor_brief::nregister()
	{
		// ��ʱ��
		actor::register_timer<actor_brief>(&actor_brief::timer_handle);

		// ���Զ���np_��Ϣ
		register_handle_custom<actor_brief>::func<
		>(true);

		// ��pb��Ϣ
		register_handle_proto<actor_brief>::func<
		>(true);
	}
	
	bool actor_brief::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_brief::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl