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
// File overview: Rank-based activity that tracks player scores and distributes ranking rewards.

#pragma once

#include "actor/actor_logic/actor_activity_manage/activity.h"
#include "actor/tab/ttab_activity_toprank.h"
#include "actor/actor_base/core/actor.h"

namespace ngl
{
	class activity_rank : 
		public activity
	{
		activity_rank(const activity_rank&) = delete;
		activity_rank& operator=(const activity_rank&) = delete;

		virtual std::shared_ptr<activity> create(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			return std::make_shared<activity_rank>(aactivityid, atime, aduration, aactivitydb, aactivitytimedb);
		}
	public:
		// Aactivityid /*activityid*/
		// Atime /*starttime*/
		// Aduration /*durationtime*/
		// Aactivitydb /*activitydata*/
		// Aactivitytimedb /*activitytimedata*/
		activity_rank(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb) :
			activity(aactivityid, atime, aduration, aactivitydb, aactivitytimedb)
		{}

		activity_rank():
			activity(EActivityTopRank)
		{}

		virtual void start();
		// Activityclose
		virtual void finish();

		void handle(const np_get_rank_response& aresponse);
	};
}//namespace ngl