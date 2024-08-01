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
						.m_area = ttab_servers::tab()->m_area,
						.m_manage_dbclient = true
					},
				.m_weight = 0x7fffffff,
			})
	{
		tdb_keyvalue::nsp_ser::init(&m_keyvaluedb);
	}

	void actor_keyvalue::nregister()
	{
	}
}//namespace ngl