#include "actor_map.h"

namespace ngl
{
	actor_map::actor_map() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MAP,
					.m_area = ttab_servers::tab()->m_area
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_map::actor_register()
	{
		// ¶¨Ê±Æ÷
		actor_map::register_timer<actor_map>(&actor_map::timer_handle);


	}

	void actor_map::loaddb_finish(bool adbishave)
	{
		LogLocalError("actor_cross###loaddb_finish#[%]", actor_guid(id_guid()));
	}

	bool actor_map::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		return true;
	}
}