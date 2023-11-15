#pragma once
#include "activitydb.h"
#include "drop.h"

namespace ngl
{
	class activity
	{
	protected:
		int64_t m_activityid;
		EActivity m_type;
		data_modified<pbdb::db_activity>* m_activity;
		pbdb::db_activity::data* m_activitydata;
	public:
		activity(int64_t activityid, data_modified<pbdb::db_activity>* aactivity) :
			m_activityid(activityid)
			, m_activity(aactivity)
		{
			tab_activity* tab = allcsv::tab<tab_activity>(activityid);
			if (tab == nullptr)
			{
				LogLocalError("tab_activity id[%] not find!!!", activityid);
				return;
			}
		}

		pbdb::db_activity::data* activitydata()
		{
			int lsize = m_activity->get().m_datas_size();
			return m_activity->get().mutable_m_datas(lsize - 1);
		}

		EActivity type()
		{
			return m_type;
		}

		virtual bool isstart()
		{
			int lcount = m_activity->getconst().m_datas_size();
			return m_activity->getconst().m_datas(lcount).m_start();
		}
		virtual bool isfinish()
		{
			int lcount = m_activity->getconst().m_datas_size();
			return m_activity->getconst().m_datas(lcount).m_finish();
		}
		// 活动开启
		virtual bool start() = 0;
		// 活动关闭
		virtual bool close() = 0;
	};


	class activity_drawcompliance : public activity
	{
	public:
		// 活动开启
		virtual bool start()
		{
			return true;
		}

		// 活动关闭
		virtual bool close()
		{
			return true;
		}

		void update(i64_actorid aroleid, int acount)
		{
			//m_drawcompliance
			pbdb::db_activity::data::drawcompliance& ldrawcompliance = 
				activitydata()->mutable_m_drawcompliance()->operator[](aroleid);
			ldrawcompliance.set_m_count(ldrawcompliance.m_count() + acount);

			
			auto lfun = [&ldrawcompliance](int acount)
				{
					for (int count : ldrawcompliance.m_reward())
					{
						if (acount == count)
							return false;
					}
					return true;
				};

			manage_csv<tab_activity_drawcompliance>* lpmanagecsv = 
				allcsv::get<manage_csv<tab_activity_drawcompliance>>();
			for (const auto& [_id, _data] : lpmanagecsv->tablecsv)
			{
				if (ldrawcompliance.m_count() > _id)
				{
					if (lfun(_data.m_id))
					{
						// 发送
						auto pro = std::make_shared<actor_protocol_add_mail>();
						pro->m_roleid = aroleid;
						pro->m_tid = _data.m_mailid;
						if (drop::droplist(_data.m_dropid, 1, pro->m_items) == false)
							continue;
						actor::static_send_actor(actor_guid::make_self(ACTOR_MAIL), actor_guid::make(), pro);

						// 记录已领取
						ldrawcompliance.add_m_reward(_data.m_id);
					}
				}
			}
		}

	};
}