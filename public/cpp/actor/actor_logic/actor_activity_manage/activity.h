#pragma once

#include "activitydb.h"
#include "drop.h"

namespace ngl
{
	class activity
	{
		activity(const activity&) = delete;
		activity& operator=(const activity&) = delete;

		static std::map<EActivity, activity*> m_activityall;
		virtual std::shared_ptr<activity> create(int32_t acalendarid, int32_t aactivityid, int64_t atime, activitydb& adb) = 0;
	public:
		static std::shared_ptr<activity> make(int32_t acalendarid, int32_t aactivityid, int64_t atime, activitydb& adb)
		{
			tab_activity* tab = allcsv::tab<tab_activity>(aactivityid);
			auto itor = m_activityall.find(tab->m_type);
			if (itor == m_activityall.end())
			{
				return nullptr;
			}
			return itor->second->create(acalendarid, aactivityid, atime, adb);
		}
	protected:
		int64_t m_activityid;
		data_modified<pbdb::db_activity>* m_activity;
	public:
		activity(int32_t acalendarid, int64_t activityid, int64_t atime, activitydb& adb);
		activity();

		pbdb::db_activity* activitydata()
		{
			return &m_activity->get();
		}

		pbdb::db_activity const* const_activitydata()
		{
			return &m_activity->getconst();
		}

		EActivity type()
		{
			tab_activity* tab = allcsv::tab<tab_activity>((int32_t)m_activityid);
			Assert(tab != nullptr);
			return tab->m_type;
		}

		virtual bool is_start()
		{
			return const_activitydata()->m_start();
		}

		virtual bool is_finish()
		{
			return const_activitydata()->m_finish();
		}

		int32_t start_utc()
		{
			return m_activity->getconst().m_beg();
		}

		int32_t finish_utc()
		{
			return m_activity->getconst().m_end();
		}

		int32_t calendarid()
		{
			return m_activity->getconst().m_calendarid();
		}

	public:

		// 活动开启
		virtual void start() {}

		virtual void rolelogin(i64_actorid aroleid){}

		// 活动关闭
		virtual void finish() {}
	};
}// namespace ngl