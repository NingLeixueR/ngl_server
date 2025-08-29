#include "actor_csvserver.h"

namespace ngl
{
	actor_csvserver::actor_csvserver() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CSVSERVER,
				},
				.m_weight = 0x7fffffff,
			})
	{}

	ENUM_ACTOR actor_csvserver::actor_type()
	{
		return ACTOR_CSVSERVER;
	}

	i64_actorid actor_csvserver::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_csvserver::init()
	{
		// ��DB�ṹ:DB.set(this);

		// ����timer_handle��ʱ��
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_csvserver::loaddb_finish(bool adbishave)
	{
	}

	void actor_csvserver::nregister()
	{
		// ��ʱ��
		actor::register_timer<actor_csvserver>(&actor_csvserver::timer_handle);

		// ���Զ���np_��Ϣ
		register_handle<actor_csvserver>::func<
			np_actor_csv_verify_version
		>(true);

		// ��pb��Ϣ
		register_handle<actor_csvserver>::func<
		>(true);
	}

	bool actor_csvserver::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_csvserver::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl