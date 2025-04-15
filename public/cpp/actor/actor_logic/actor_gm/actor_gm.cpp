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

	void actor_gm::loaddb_finish(bool adbishave) 
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
		return ttab_servers::tab()->m_type == atype;
	}

	void actor_gm::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_gm>::func<
			ngl::np_gm
			, mforward<ngl::np_gm_response>
		>(false);
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
		assert(m_actor != nullptr);
		assert(m_actor->id_guid() == actor_gm::getInstance().id_guid());
		actor_gm::sendtophp(apro->identifier(), *apro->data());
	}
}// namespace ngl