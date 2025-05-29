#include "activity_drawcompliance.h"
#include "actor_activity_manage.h"
#include "activity.h"

namespace ngl
{
	std::map<EActivity, activity*> activity::m_activityall;
	activity_drawcompliance g_activity_drawcompliance;

	activity::activity()
	{

	}

	activity::activity(int32_t activityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
	{
		m_tab = ttab_activity::tab(activityid);
		assert(m_tab != nullptr);

		m_activity = aactivitydb.get_activity(activityid);
		if (m_activity == nullptr)
		{
			pbdb::db_activity ldb;
			ldb.set_m_id(activityid);
			m_activity = aactivitydb.add(activityid, ldb);
		}
		m_activitytimes = aactivitytimedb.get_activity(activityid);
		if (m_activitytimes == nullptr)
		{
			pbdb::db_activitytimes ldb;
			ldb.set_m_id(activityid);
			ldb.set_m_beg(atime);
			ldb.set_m_duration(aduration);
			ldb.set_m_start(false);
			ldb.set_m_finish(false);
			m_activitytimes = aactivitytimedb.add(activityid, ldb);
		}
	}	

	void activity::rolelogin(i64_actorid aroleid)
	{
		log_error()->print("activity::rolelogin() activityid=[{}] roleid=[{}]", activityid(), aroleid);
		const tab_activity* ltab = tab();
		int32_t lday = day();
		auto& ldb_activity = m_activity->getconst();

		auto itor = ldb_activity.m_task().find(aroleid);
		if (itor == ldb_activity.m_task().end())
		{
			(*m_activity->get().mutable_m_task())[aroleid];
			itor = ldb_activity.m_task().find(aroleid);
		}
		const pbdb::activity_task& ltask = itor->second;

		for (int32_t i = 0; i < ltab->m_taskday.size(); ++i)
		{
			auto& item = ltab->m_taskday[i];
			// 开启任务
			if (lday >= item.m_begday && lday < item.m_endday)
			{
				auto itoropen = ltask.m_open().find(i);
				if (itoropen == ltask.m_open().end())
				{
					auto pro = std::make_shared<mforward<np_operator_task>>(actor_activity_manage::actorid());
					np_operator_task* lnp = pro->add_data();
					lnp->m_isreceive = true;
					lnp->m_taskids = item.m_taskids;
					lnp->m_msg = std::format("{}:{}", activityid(), i);
					actor::static_send_actor(aroleid, nguid::make(), pro);
				}
			}
			// 关闭任务
			if (lday >= item.m_endday)
			{
				auto itoropen = ltask.m_open().find(i);
				auto itorclose = ltask.m_close().find(i);
				if (itoropen != ltask.m_open().end() && itorclose == ltask.m_close().end())
				{
					auto pro = std::make_shared<mforward<np_operator_task>>(actor_activity_manage::actorid());
					np_operator_task* lnp = pro->add_data();
					lnp->m_isreceive = false;
					lnp->m_taskids = item.m_taskids;
					lnp->m_msg = std::format("{}:{}", activityid(), i);
					actor::static_send_actor(aroleid, nguid::make(), pro);
				}
			}
		}
	}

	void activity::rolelevelchange(i64_actorid aroleid, int32_t abeforelevel, int32_t anowlevel)
	{
		if (!is_rank())
		{
			return;
		}
		const tab_activity* lpactivity = tab();
		if (lpactivity == nullptr)
		{
			return;
		}

		actor_activity_manage::getInstance().log_error()->print(
			"activity [{}] rolelevelchange roleid:[{}] beforelevel[{}] nowlevel[{}]",
			activityid(), aroleid, abeforelevel, anowlevel
		);

		auto itor = std::ranges::find_if(lpactivity->m_ranktype, [](int atype)
			{
				return atype == pbdb::eranklist::activity_lv;
			});
		if (itor == lpactivity->m_ranktype.end())
		{
			return;
		}
		// 活动期间产生等级变化
		pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_activity_manage>::getInstance().get(aroleid);
		if (lpbrief != nullptr)
		{
			(*lpbrief->mutable_m_activityvalues()->mutable_m_activity_rolelv())[activityid()] += anowlevel - abeforelevel;
		}
	}

	void activity::rolegoldchange(i64_actorid aroleid, int32_t abeforegold, int32_t anowgold)
	{
		if (!is_rank())
		{
			return;
		}
		const tab_activity* lpactivity = tab();
		if (lpactivity == nullptr)
		{
			return;
		}

		actor_activity_manage::getInstance().log_error()->print(
			"activity [{}] rolegoldchange roleid:[{}] beforegold[{}] nowgold[{}]",
			activityid(), aroleid, abeforegold, anowgold
		);

		auto itor = std::ranges::find_if(lpactivity->m_ranktype, [](int atype)
			{
				return atype == pbdb::eranklist::activity_lv;
			});
		if (itor == lpactivity->m_ranktype.end())
		{
			return;
		}
		// 活动期间产生等级变化
		pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_activity_manage>::getInstance().get(aroleid);
		if (lpbrief != nullptr)
		{
			(*lpbrief->mutable_m_activityvalues()->mutable_m_activity_rolegold())[activityid()] += anowgold - abeforegold;
		}
	}

	void activity::brief_activityvalues(i64_actorid aroleid)
	{
		tdb_brief::nsp_cli<actor_activity_manage>::getInstance().change(aroleid);
	}

}//namespace ngl