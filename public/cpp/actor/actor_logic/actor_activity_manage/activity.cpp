#include "activity_drawcompliance.h"
#include "actor_activity_manage.h"
#include "activity_rank.h"
#include "activity.h"

namespace ngl
{
	std::map<EActivity, activity*> activity::m_activityall;
	activity_drawcompliance g_activity_drawcompliance;
	activity_rank g_activity_rank;

	activity::activity(
		int32_t activityid
		, int32_t atime
		, int32_t aduration
		, activitydb& aactivitydb
		, activitytimedb& aactivitytimedb
	):
		m_activitydb(&aactivitydb),
		m_activitytimedb(&aactivitytimedb)
	{
		m_tab = ttab_activity::instance().tab(activityid);
		if (m_tab == nullptr)
		{
			tools::no_core_dump();
			return;
		}

		m_actorid = actor::tab2actor(ACTOR_ACTIVITY_MANAGE, activityid);

		m_activity = aactivitydb.find(m_actorid);
		if (m_activity == nullptr)
		{
			pbdb::db_activity ldb;
			ldb.set_m_id(m_actorid);
			m_activity = &aactivitydb.add(m_actorid, ldb);
		}
		m_activitytimes = aactivitytimedb.find(m_actorid);
		if (m_activitytimes == nullptr)
		{
			pbdb::db_activitytimes ldb;
			ldb.set_m_id(m_actorid);
			ldb.set_m_beg(atime);
			ldb.set_m_duration(aduration);
			ldb.set_m_start(false);
			m_activitytimes = &aactivitytimedb.add(m_actorid, ldb);
		}
	}	

	activity::activity(EActivity atype)
	{
		m_activityall[atype] = this;
	}

	void activity::rolelogin(i64_actorid aroleid)
	{
		log_error()->print(
			"activity::rolelogin() activityid=[{}] roleid=[{}]", 
			activityid(), aroleid
		);
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
					actor::send_actor(aroleid, nguid::make(), pro);
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
					actor::send_actor(aroleid, nguid::make(), pro);
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

		actor_activity_manage::instance().log_error()->print(
			"activity [{}] rolelevelchange roleid:[{}] beforelevel[{}] nowlevel[{}]",
			nguid(activityid()), aroleid, abeforelevel, anowlevel
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
		pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_activity_manage>::instance(actor_activity_manage::actorid()).get(aroleid);
		if (lpbrief != nullptr)
		{
			(*lpbrief->mutable_m_activityvalues()->mutable_m_activity_rolelv())[lpactivity->m_id] += anowlevel - abeforelevel;
		}
		brief_activityvalues(aroleid);
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

		actor_activity_manage::instance().log_error()->print(
			"activity [{}] rolegoldchange roleid:[{}] beforegold[{}] nowgold[{}]",
			nguid(activityid()), aroleid, abeforegold, anowgold
		);

		auto itor = std::ranges::find_if(lpactivity->m_ranktype, [](int atype)
			{
				return atype == pbdb::eranklist::activity_gold;
			});
		if (itor == lpactivity->m_ranktype.end())
		{
			return;
		}
		// 活动期间产生等级变化
		pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_activity_manage>::instance(actor_activity_manage::actorid()).get(aroleid);
		if (lpbrief != nullptr)
		{
			(*lpbrief->mutable_m_activityvalues()->mutable_m_activity_rolegold())[lpactivity->m_id] += anowgold - abeforegold;
		}
		brief_activityvalues(aroleid);
	}

	void activity::brief_activityvalues(i64_actorid aroleid)
	{
		tdb_brief::nsp_cli<actor_activity_manage>::instance(actor_activity_manage::actorid()).change(aroleid);
	}
}//namespace ngl