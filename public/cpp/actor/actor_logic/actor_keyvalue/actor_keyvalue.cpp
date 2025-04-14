#include "actor_keyvalue.h"
#include "nsp_server.h"
#include "net.pb.h"

namespace ngl
{
	actor_keyvalue::actor_keyvalue() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_KEYVALUE,
						.m_area = tab_self_area,
						.m_manage_dbclient = true
					},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		tdb_keyvalue::nsp_ser::init(&m_keyvaluedb);
	}

	void actor_keyvalue::init() final
	{
		m_keyvaluedb.set(this);
	}

	void actor_keyvalue::loaddb_finish(bool adbishave) final
	{
		log_error()->print("actor_keyvalue load finish!!!");
	}

	ENUM_ACTOR actor_keyvalue::actor_type()
	{
		return ACTOR_KEYVALUE;
	}

	i64_actorid actor_keyvalue::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_keyvalue::nregister()
	{
	}
}//namespace ngl