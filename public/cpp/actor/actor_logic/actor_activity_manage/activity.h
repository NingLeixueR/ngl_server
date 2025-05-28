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
		virtual std::shared_ptr<activity> create(int32_t aactivityid, int64_t atime, activitydb& aactivitydb, activitytimedb& aactivitytimedb) = 0;
	public:
		static std::shared_ptr<activity> make(int32_t aactivityid, int64_t atime, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			tab_activity* tab = allcsv::tab<tab_activity>(aactivityid);
			auto itor = m_activityall.find(tab->m_type);
			if (itor == m_activityall.end())
			{
				return nullptr;
			}
			return itor->second->create(aactivityid, atime, aactivitydb, aactivitytimedb);
		}
	protected:
		tab_activity* m_tab = nullptr;
		data_modified<pbdb::db_activity>* m_activity = nullptr;
		data_modified<pbdb::db_activitytimes>* m_activitytimes = nullptr;
	public:
		activity(int64_t activityid, int64_t atime, activitydb& aactivitydb, activitytimedb& aactivitytimedb);
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
			return m_activitytimes->getconst().m_end();
		}

		// # �˿��ǻ�ڼ���
		int32_t day()
		{
			return localtime::getspandays(localtime::gettime(), start_utc());
		}

		const tab_activity* tab()
		{
			return m_tab;
		}

		// # �Ƿ�֧�����а�
		bool is_rank()
		{
			return activityid() >= 1000 || activityid() < 2000;
		}
	public:
		// # ����:�����
		virtual void start() 
		{
			log_error()->print("activity::start() activityid=[{}]", activityid());
			m_activitytimes->get().set_m_start(true);
		}

		// # ����:�������ͷ���������
		virtual void init() 
		{
			log_error()->print("activity::init() activityid=[{}]", activityid());
		}

		// # ����:��ҵ�½
		virtual void rolelogin(i64_actorid aroleid);

		// # ����:��ҿ�����ɹ�
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

		// # ����:��ҵȼ������仯
		virtual void rolelevelchange(i64_actorid aroleid, int32_t abeforelevel, int32_t anowlevel);

		// # ����:��ҽ�ҷ����仯
		virtual void rolegoldchange(i64_actorid aroleid, int32_t abeforegold, int32_t anowgold);

		// # db_brief.m_activityvalues�����仯�����
		static void brief_activityvalues(i64_actorid aroleid);

		// # ����:��ر�
		virtual void finish() 
		{

		}
	};
}// namespace ngl