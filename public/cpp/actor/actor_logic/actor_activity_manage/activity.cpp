/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		m_actorid = nguid::make(ACTOR_ACTIVITY_MANAGE, nguid::none_area(), activityid);

		m_activity = aactivitydb.find(m_actorid);
		if (m_activity == nullptr)
		{
			pbdb::db_activity ldb;
			ldb.set_mid(m_actorid);
			m_activity = &aactivitydb.add(m_actorid, ldb);
		}
		m_activitytimes = aactivitytimedb.find(m_actorid);
		if (m_activitytimes == nullptr)
		{
			pbdb::db_activitytimes ldb;
			ldb.set_mid(m_actorid);
			ldb.set_mbeg(atime);
			ldb.set_mduration(aduration);
			ldb.set_mstart(false);
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
		const pbdb::db_activity* lpdbactivityconst = m_activity->getconst();
		if (lpdbactivityconst == nullptr)
		{
			return;
		}
		auto itor = lpdbactivityconst->mtask().find(aroleid);
		if (itor == lpdbactivityconst->mtask().end())
		{
			pbdb::db_activity* lpdbactivity = m_activity->get();
			if (lpdbactivity == nullptr)
			{
				return;
			}
			(*lpdbactivity->mutable_mtask())[aroleid];
			itor = lpdbactivityconst->mtask().find(aroleid);
		}

		const pbdb::activity_task& ltask = itor->second;
		for (int32_t i = 0; i < ltab->m_taskday.size(); ++i)
		{
			auto& item = ltab->m_taskday[i];
			// 开启任务
			if (lday >= item.m_begday && lday < item.m_endday)
			{
				auto itoropen = ltask.mopen().find(i);
				if (itoropen == ltask.mopen().end())
				{
					auto pro = std::make_shared<mforward<np_operator_task>>(actor_activity_manage::actorid());
					np_operator_task* lnp = pro->data();
					lnp->m_isreceive = true;
					lnp->m_taskids = item.m_taskids;
					lnp->m_msg = std::format("{}:{}", activityid(), i);
					actor::send_actor(aroleid, nguid::make(), pro);
				}
			}
			// 关闭任务
			if (lday >= item.m_endday)
			{
				auto itoropen = ltask.mopen().find(i);
				auto itorclose = ltask.mclose().find(i);
				if (itoropen != ltask.mopen().end() && itorclose == ltask.mclose().end())
				{
					auto pro = std::make_shared<mforward<np_operator_task>>(actor_activity_manage::actorid());
					np_operator_task* lnp = pro->data();
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

		log_error()->print(
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
		pbdb::db_brief* lpbrief = tdb_brief::nsp_cwrite<actor_activity_manage>::instance(actor_activity_manage::actorid()).get(aroleid);
		if (lpbrief != nullptr)
		{
			(*lpbrief->mutable_mactivityvalues()->mutable_mactivity_rolelv())[lpactivity->m_id] += anowlevel - abeforelevel;
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

		log_error()->print(
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
		pbdb::db_brief* lpbrief = tdb_brief::nsp_cwrite<actor_activity_manage>::instance(actor_activity_manage::actorid()).get(aroleid);
		if (lpbrief != nullptr)
		{
			(*lpbrief->mutable_mactivityvalues()->mutable_mactivity_rolegold())[lpactivity->m_id] += anowgold - abeforegold;
		}
		brief_activityvalues(aroleid);
	}

	void activity::brief_activityvalues(i64_actorid aroleid)
	{
		tdb_brief::nsp_cwrite<actor_activity_manage>::instance(actor_activity_manage::actorid()).change();
	}
}//namespace ngl