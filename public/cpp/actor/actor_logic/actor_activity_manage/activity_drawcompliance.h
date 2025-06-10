#pragma once
#include "ttab_activity_drawcompliance.h"
#include "activity.h"

namespace ngl
{
	class activity_drawcompliance : 
		public activity
	{
		activity_drawcompliance(const activity_drawcompliance&) = delete;
		activity_drawcompliance& operator=(const activity_drawcompliance&) = delete;

		virtual std::shared_ptr<activity> create(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			return std::make_shared<activity_drawcompliance>(
				aactivityid, atime, aduration, aactivitydb, aactivitytimedb
			);
		}

	public:

		activity_drawcompliance(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb) :
			activity(aactivityid, atime, aduration, aactivitydb, aactivitytimedb)
		{}

		activity_drawcompliance():
			activity(EActivityDrawCompliance)
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
				pbdb::activity_drawcompliance ltemp;
				ltemp.set_m_count(acount);
				itor = lmap->insert({ aroleid, ltemp }).first;
			}

			for (const auto& [_id, _data] : ttab_activity_drawcompliance::tablecsv())
			{
				if (itor->second.m_count() > _id)
				{
					auto itorreward = itor->second.mutable_m_reward()->find(_id);
					if (itorreward == itor->second.mutable_m_reward()->end())
					{
						// 发送
						std::string lsrc = std::format(
							"activity_drawcompliance role=[{}] mail=[{}] drop=[{}]", 
							aroleid, _data.m_mailid, _data.m_dropid
						);
						if (actor_activity_manage::get_drop().use(_data.m_dropid, 1, aroleid, lsrc, nullptr, _data.m_mailid))
						{
							// 记录已领取
							itor->second.mutable_m_reward()->insert({ _id, true });
						}
					}
				}
			}
		}
	};
}