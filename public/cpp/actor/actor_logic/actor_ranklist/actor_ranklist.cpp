#include "actor_ranklist.h"
#include "actor_brief.h"

namespace ngl
{
	actor_ranklist::actor_ranklist() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_RANKLIST,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		std::set<i64_actorid> ldataid;
		tdb_brief::nsp_cli<actor_ranklist>::init(actor_brief::actor_type(), this, ldataid);
	}

	ENUM_ACTOR actor_ranklist::actor_type()
	{
		return ACTOR_RANKLIST;
	}

	i64_actorid actor_ranklist::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_ranklist::init()
	{
		// ��DB�ṹ:DB.set(this);
		m_ranklist.set(this);

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

	void actor_ranklist::loaddb_finish(bool adbishave)
	{
	}

	void actor_ranklist::nregister()
	{
		// ��ʱ��
		actor::register_timer<actor_ranklist>(&actor_ranklist::timer_handle);

		// ���Զ���np_��Ϣ
		register_handle_custom<actor_ranklist>::func<
			mforward<np_gm>
		>(true);

		// ��pb��Ϣ
		register_handle_proto<actor_ranklist>::func<
			mforward<pbnet::PROBUFF_NET_RANKLIST>
		>(true);
	}

	bool actor_ranklist::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_ranklist::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl