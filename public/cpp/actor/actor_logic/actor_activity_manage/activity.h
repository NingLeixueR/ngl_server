#pragma once

#include "activitytimedb.h"
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
		virtual std::shared_ptr<activity> create(
			int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb
		) = 0;
	public:
		static std::shared_ptr<activity> make(
			int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb
		)
		{
			const tab_activity* ltab = ttab_activity::tab(aactivityid);
			auto itor = m_activityall.find(ltab->m_type);
			if (itor == m_activityall.end())
			{
				return nullptr;
			}
			return itor->second->create(aactivityid, atime, aduration, aactivitydb, aactivitytimedb);
		}
	protected:
		const tab_activity* m_tab = nullptr;
		data_modified<pbdb::db_activity>* m_activity = nullptr;
		data_modified<pbdb::db_activitytimes>* m_activitytimes = nullptr;

	public:
		activity(int32_t activityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb);
		activity();

		EActivity type()
		{
			return m_tab->m_type;
		}

		int64_t activityid()
		{
			return m_tab->m_id;
		}

		virtual bool is_start()
		{
			return m_activitytimes->getconst().m_start();
		}

		virtual bool is_finish()
		{
			return m_activitytimes->getconst().m_finish();
		}

		int32_t start_utc()
		{
			return m_activitytimes->getconst().m_beg();
		}

		int32_t finish_utc()
		{
			if (m_activitytimes->getconst().m_duration() == -1)
			{
				return -1;
			}
			return start_utc() + m_activitytimes->getconst().m_duration();
		}

		// # 此刻是活动第几天
		int32_t day()
		{
			return localtime::getspandays(localtime::gettime(), start_utc());
		}

		const tab_activity* tab()
		{
			return m_tab;
		}

		// # 是否支持排行榜
		bool is_rank()
		{
			return activityid() >= 1000 || activityid() < 2000;
		}
	public:
		// # 调用:活动开启
		virtual void start() 
		{
			log_error()->print("activity::start() activityid=[{}]", activityid());
			m_activitytimes->get().set_m_start(true);
		}

		// # 调用:活动开启后和服务器重启
		virtual void init() 
		{
			log_error()->print("activity::init() activityid=[{}]", activityid());
		}

		// # 调用:玩家登陆
		virtual void rolelogin(i64_actorid aroleid);

		// # 调用:玩家开启活动成功
		void recv_task_response(i64_actorid aroleid, int32_t aindex, bool aisreceive)
		{
			if (aisreceive)
			{
				(*(*m_activity->get().mutable_m_task())[aroleid].mutable_m_open())[aindex] = true;
			}
			else
			{
				(*(*m_activity->get().mutable_m_task())[aroleid].mutable_m_close())[aindex] = true;
			}
		}

		// # 调用:玩家等级发生变化
		virtual void rolelevelchange(i64_actorid aroleid, int32_t abeforelevel, int32_t anowlevel);

		// # 调用:玩家金币发生变化
		virtual void rolegoldchange(i64_actorid aroleid, int32_t abeforegold, int32_t anowgold);

		// # db_brief.m_activityvalues发生变化后调用
		static void brief_activityvalues(i64_actorid aroleid);

		// # 调用:活动关闭
		virtual void finish() 
		{
			log_error()->print("activity::finish() activityid=[{}]", activityid());
			m_activitytimes->get().set_m_start(false);
		}
	};
}// namespace ngl