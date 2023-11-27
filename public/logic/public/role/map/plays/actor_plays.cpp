#include "actor_plays.h"
#include "matching.h"

namespace ngl
{
	actor_plays::actor_plays(ENUM_ACTOR atype, int32_t adataid, void* data) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = atype,
					.m_area = ttab_servers::tab()->m_area,
					.m_id = adataid
				},
				.m_weight = 10,
			})
	{

		

		int32_t tid = roomid::tid(adataid);
		auto tab = allcsv::tab<tab_plays>(tid);	
		assert(tab != nullptr);

		// 告诉匹配房间可以解散了，这边已经建立好玩法了
		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>();
		pro->set_m_type((pbnet::eplays)tab->m_type);
		pro->set_m_roomid(adataid);
		send_actor(actor_guid::make_self(ACTOR_MATCHING), pro);

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