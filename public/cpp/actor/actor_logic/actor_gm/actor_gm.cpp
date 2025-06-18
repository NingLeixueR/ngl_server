#include "actor_gmclient.h"
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
					.m_area = tab_self_area,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_gm::actor_type()
	{
		return ACTOR_GM;
	}

	i64_actorid actor_gm::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, nguid::none_actordataid());
	}

	void actor_gm::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gm::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}
	
	void actor_gm::loaddb_finish(bool adbishave)
	{
	}

	void actor_gm::nregister()
	{
		// 定时器
		actor::register_timer<actor_gm>(&actor_gm::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_gm, false>::func<
			ngl::np_gm
			, mforward<ngl::np_gm_response>
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_gm, false>::func<
		>(true);
	}

	void actor_gm::sendbytype(ENUM_ACTOR atype, const pack* apack, const ngl::np_gm& apro)
	{
		auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
		send_actor(atype, pro, true);
	}

	void actor_gm::sendbyactorid(i64_actorid aactorid, const pack* apack, const ngl::np_gm& apro)
	{
		auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
		send_actor(aactorid, pro);
	}

	bool actor_gm::checklocalbytype(NODE_TYPE atype)
	{
		return ttab_servers::instance().tab()->m_type == atype;
	}

	bool actor_gm::sendtophp(i32_sessionid aidentifier, const ngl::np_gm_response& adata)
	{
		::ngl::log_error()->print("gm2php [{}]", adata.m_json);
		actor::send(aidentifier, adata, nguid::make(), nguid::make());
		return true;
	}

	template <typename T>
	void gcmd<T>::execute(std::shared_ptr<mforward<ngl::np_gm_response>>& apro)
	{
		if (m_actor == nullptr || m_actor->id_guid() != actor_gm::instance().id_guid())
		{
			tools::no_core_dump();
			return;
		}
		actor_gm::sendtophp(apro->identifier(), *apro->data());
	}

	bool actor_gm::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_gm::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl