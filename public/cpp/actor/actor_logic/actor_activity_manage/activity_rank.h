#pragma once
#include "ttab_activity_toprank.h"
#include "activity.h"
#include "actor.h"

namespace ngl
{
	class activity_rank : 
		public activity
	{
		activity_rank(const activity_rank&) = delete;
		activity_rank& operator=(const activity_rank&) = delete;

		virtual std::shared_ptr<activity> create(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			return std::make_shared<activity_rank>(
				aactivityid, atime, aduration, aactivitydb, aactivitytimedb
			);
		}

	public:

		activity_rank(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb) :
			activity(aactivityid, atime, aduration, aactivitydb, aactivitytimedb)
		{}

		activity_rank():
			activity(EActivityTopRank)
		{}

		virtual void start();
		// »î¶¯¹Ø±Õ
		virtual void finish();

		void handle(const np_get_rank_response& aresponse);
	};
}