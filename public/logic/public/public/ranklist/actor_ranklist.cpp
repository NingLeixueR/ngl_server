#include "actor_ranklist.h"


namespace ngl
{

	actor_ranklist::actor_ranklist() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_RANKLIST,
					.m_area = ttab_servers::tab()->m_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
		type_roleitems::init(
			[](std::map<i64_actorid, pbdb::db_brief>& amap, pbdb::db_brief& ainfo)
			{
				amap[ainfo.m_id()] = ainfo;
			}
		);
	}

}