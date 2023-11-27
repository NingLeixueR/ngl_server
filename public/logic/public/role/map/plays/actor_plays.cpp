#include "actor_plays.h"
#include "matching.h"

namespace ngl
{
	actor_plays::actor_plays(int32_t adataid, void* data) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_PLAYS,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = adataid
				},
				.m_weight = 10,
			})
	{
		int32_t tid = roomid::tid(adataid);
		auto tab = allcsv::tab<tab_plays>(tid);	
		assert(tab != nullptr);
		m_map.init(tab->m_mapid);

		std::vector<i64_actorid>& lplays = ((actor_switch_process_plays*)data)->m_players;
		m_map.set_enterview([this](i64_actorid aroleid, std::set<i64_actorid>& aother)
			{
				player_enterview(aroleid, aother);
			});
		m_map.set_leaveview([this](i64_actorid aroleid, std::set<i64_actorid>& aother)
			{
				player_leaveview(aroleid, aother);
			});

		init_after();
		// 开始准备阶段
		m_stage = estage_preparation;
		preparation(true);
	}

	void actor_plays::actor_register()
	{
		// 定时器
		register_timer<actor_plays>(&actor_plays::timer_handle);
	}
}