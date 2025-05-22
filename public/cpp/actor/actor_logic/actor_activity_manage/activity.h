#pragma once

#include "ttab_activity.h"
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

		EActivity type()
		{
			tab_activity* tab = allcsv::tab<tab_activity>((int32_t)m_activityid);
			Assert(tab != nullptr);
			return tab->m_type;
		}

		int64_t activityid()
		{
			return m_activityid;
		}

		virtual bool is_start()
		{
			return m_activity->getconst().m_start();
		}

		virtual bool is_finish()
		{
			return m_activity->getconst().m_finish();
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

		// # 此刻是活动第几天
		int32_t day()
		{
			return localtime::getspandays(localtime::gettime(), start_utc());
		}

		const tab_activity* tab()
		{
			const tab_activity* ltab = ttab_activity::tab(m_activityid);
			assert(ltab != nullptr);
			return ltab;
		}
	public:
		// # 调用:活动开启
		virtual void start() {}

		// # 调用:活动开启后和服务器重启
		virtual void init() {}

		// # 调用:玩家登陆
		virtual void rolelogin(i64_actorid aroleid);

		// # 调用:玩家开启活动成功
		void recv_task_response(i64_actorid aroleid, int32_t aindex, bool aisreceive)
		{
			if (aisreceive)
			{
				(*(*m_activity->get().mutable_m_stat())[aroleid].mutable_m_opentask())[aindex] = true;
			}
			else
			{
				(*(*m_activity->get().mutable_m_stat())[aroleid].mutable_m_closetask())[aindex] = true;
			}
		}

		// # 调用:活动关闭
		virtual void finish() 
		{

		}
	};
}// namespace ngl