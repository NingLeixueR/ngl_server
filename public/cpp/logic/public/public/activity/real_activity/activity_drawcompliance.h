#pragma once
#include "activity.h"

namespace ngl
{
	class activity_drawcompliance : public activity
	{
		activity_drawcompliance(const activity_drawcompliance&) = delete;
		activity_drawcompliance& operator=(const activity_drawcompliance&) = delete;

		virtual std::shared_ptr<activity> create(
			int32_t acalendarid, 
			int32_t aactivityid, 
			int64_t atime, 
			activitydb& adb
		)
		{
			std::shared_ptr<activity> ret = std::make_shared<activity_drawcompliance>(
				acalendarid, aactivityid, atime, adb
			);
			return ret;
		}
	public:

		activity_drawcompliance(
			int32_t acalendarid, 
			int32_t aactivityid, 
			int64_t atime, 
			activitydb& adb
		) :
			activity(acalendarid, aactivityid, atime, adb)
		{}

		activity_drawcompliance()
		{}

		// 活动开启
		virtual void start()
		{
			return;
		}

		// 活动关闭
		virtual void finish()
		{
			return;
		}

		void update(i64_actorid aroleid, int acount)
		{
			pbdb::db_activity& lactivitydb = m_activity->get();
			auto lmap = lactivitydb.mutable_m_drawcompliance();
			auto itor = lmap->find(aroleid);
			if (itor == lmap->end())
			{
				pbdb::db_activity::drawcompliance ltemp;
				ltemp.set_m_count(acount);
				itor = lmap->insert({ aroleid, ltemp }).first;
			}

			manage_csv<tab_activity_drawcompliance>* lpmanagecsv =
				allcsv::get<manage_csv<tab_activity_drawcompliance>>();
			for (const auto& [_id, _data] : lpmanagecsv->m_tablecsv)
			{
				if (itor->second.m_count() > _id)
				{
					auto itorreward = itor->second.mutable_m_reward()->find(_id);
					if (itorreward == itor->second.mutable_m_reward()->end())
					{
						// 发送
						auto pro = std::make_shared<np_actor_addmail>();
						pro->m_roleid = aroleid;
						pro->m_tid = _data.m_mailid;
						if (drop::droplist(_data.m_dropid, 1, pro->m_items) == false)
							continue;
						actor::static_send_actor(nguid::make_self(ACTOR_MAIL), nguid::make(), pro);

						// 记录已领取
						itor->second.mutable_m_reward()->insert({ _id, true });
					}
				}
			}
		}
	};
}