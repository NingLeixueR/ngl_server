/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		virtual std::shared_ptr<activity> create(
			int32_t aactivityid
			, int32_t atime
			, int32_t aduration
			, activitydb& aactivitydb
			, activitytimedb& aactivitytimedb
		)
		{
			return std::make_shared<activity_drawcompliance>(
				aactivityid, atime, aduration, aactivitydb, aactivitytimedb
			);
		}

	public:
		activity_drawcompliance(
			int32_t aactivityid					/*活动id*/
			, int32_t atime						/*开启时间*/
			, int32_t aduration					/*持续时间*/
			, activitydb& aactivitydb			/*活动数据*/
			, activitytimedb& aactivitytimedb	/*活动时间数据*/
		) :
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
			pbdb::db_activity* lpdbactivity = m_activity->get();
			if (lpdbactivity == nullptr)
			{
				return;
			}
			auto lmap = lpdbactivity->mutable_mdrawcompliance();
			auto itor = lmap->find(aroleid);
			if (itor == lmap->end())
			{
				pbdb::activity_drawcompliance ltemp;
				ltemp.set_mcount(acount);
				itor = lmap->insert({ aroleid, ltemp }).first;
			}

			for (const auto& [_id, _data] : ttab_activity_drawcompliance::instance().tablecsv())
			{
				if (itor->second.mcount() > _id)
				{
					auto itorreward = itor->second.mutable_mreward()->find(_id);
					if (itorreward == itor->second.mutable_mreward()->end())
					{
						// 发送
						std::string lsrc = std::format(
							"activity_drawcompliance role=[{}] mail=[{}] drop=[{}]", 
							aroleid, _data.m_mailid, _data.m_dropid
						);
						if (actor_activity_manage::get_drop().use(_data.m_dropid, 1, aroleid, lsrc, nullptr, _data.m_mailid))
						{
							// 记录已领取
							itor->second.mutable_mreward()->insert({ _id, true });
						}
					}
				}
			}
		}
	};
}//namespace ngl