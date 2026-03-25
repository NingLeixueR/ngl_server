/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for actor activity manage.

#pragma once

#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_activity_manage/activity.h"
#include "actor/tab/ttab_activity_drawcompliance.h"

namespace ngl
{
	class activity_drawcompliance : 
		public activity
	{
		activity_drawcompliance(const activity_drawcompliance&) = delete;
		activity_drawcompliance& operator=(const activity_drawcompliance&) = delete;

		virtual std::shared_ptr<activity> create(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			return std::make_shared<activity_drawcompliance>(aactivityid, atime, aduration, aactivitydb, aactivitytimedb);
		}

	public:
		// Aactivityid /*activityid*/
		// Atime /*starttime*/
		// Aduration /*durationtime*/
		// Aactivitydb /*activitydata*/
		// Aactivitytimedb /*activitytimedata*/
		activity_drawcompliance(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb) :
			activity(aactivityid, atime, aduration, aactivitydb, aactivitytimedb)
		{}

		activity_drawcompliance():
			activity(EActivityDrawCompliance)
		{}

		// Activitystart
		virtual void start()
		{
			return;
		}

		// Activityclose
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
			auto* lmap = lpdbactivity->mutable_mdrawcompliance();
			auto lit = lmap->find(aroleid);
			if (lit == lmap->end())
			{
				pbdb::activity_drawcompliance ltemp;
				ltemp.set_mcount(acount);
				lit = lmap->insert({ aroleid, ltemp }).first;
			}
			auto& ldraw = lit->second;
			auto* lreward = ldraw.mutable_mreward();
			const int lcount = ldraw.mcount();

			ttab_activity_drawcompliance::instance().foreach([&](tab_activity_drawcompliance& atab)
				{
					if (lcount > atab.m_id)
					{
						auto litreward = lreward->find(atab.m_id);
						if (litreward == lreward->end())
						{
							// Send
							std::string lsrc = std::format("activity_drawcompliance role=[{}] mail=[{}] drop=[{}]", aroleid, atab.m_mailid, atab.m_dropid);
							if (actor_activity_manage::get_drop().use(atab.m_dropid, 1, aroleid, lsrc, nullptr, atab.m_mailid))
							{
								// Record claim
								lreward->insert({ atab.m_id, true });
							}
						}
					}
				}
			);
		}
	};
}//namespace ngl
