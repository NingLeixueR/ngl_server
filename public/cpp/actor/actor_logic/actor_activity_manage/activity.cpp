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
// File overview: Implements logic for actor activity manage.


#include "actor/actor_logic/actor_activity_manage/activity_drawcompliance.h"
#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_activity_manage/activity_rank.h"
#include "actor/actor_logic/actor_activity_manage/activity.h"

namespace ngl
{
	std::map<EActivity, activity*> activity::m_activityall;
	activity_drawcompliance g_activity_drawcompliance;
	activity_rank g_activity_rank;

	activity::activity(int32_t activityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb):
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
		m_activityall.insert_or_assign(atype, this);
	}

	void activity::rolelogin(i64_actorid aroleid)
	{
		log_error()->print("activity::rolelogin() activityid=[{}] roleid=[{}]", activityid(), aroleid);
		const tab_activity* ltab = tab();
		int32_t lday = day();
		const pbdb::db_activity* lpdbactivityconst = m_activity->getconst();
		if (lpdbactivityconst == nullptr)
		{
			return;
		}
		const pbdb::activity_task* lptask = tools::findmap(lpdbactivityconst->mtask(), aroleid);
		if (lptask == nullptr)
		{
			pbdb::db_activity* lpdbactivity = m_activity->get();
			if (lpdbactivity == nullptr)
			{
				return;
			}
			auto* ltaskmap = lpdbactivity->mutable_mtask();
			(*ltaskmap)[aroleid];
			lptask = tools::findmap(*ltaskmap, aroleid);
			if (lptask == nullptr)
			{
				return;
			}
		}

		const pbdb::activity_task& ltask = *lptask;
		int32_t ltaskidx = 0;
		for (const auto& litem : ltab->m_taskday)
		{
			// Starttask
			if (lday >= litem.m_begday && lday < litem.m_endday)
			{
				auto itoropen = ltask.mopen().find(ltaskidx);
				if (itoropen == ltask.mopen().end())
				{
					auto pro = std::make_shared<mforward<np_operator_task>>(actor_activity_manage::actorid());
					np_operator_task* lnp = pro->data();
					lnp->m_isreceive = true;
					lnp->m_taskids = litem.m_taskids;
					lnp->m_msg = std::format("{}:{}", activityid(), ltaskidx);
					actor::send_actor(aroleid, nguid::make(), pro);
				}
			}
			// Closetask
			if (lday >= litem.m_endday)
			{
				auto itoropen = ltask.mopen().find(ltaskidx);
				auto itorclose = ltask.mclose().find(ltaskidx);
				if (itoropen != ltask.mopen().end() && itorclose == ltask.mclose().end())
				{
					auto pro = std::make_shared<mforward<np_operator_task>>(actor_activity_manage::actorid());
					np_operator_task* lnp = pro->data();
					lnp->m_isreceive = false;
					lnp->m_taskids = litem.m_taskids;
					lnp->m_msg = std::format("{}:{}", activityid(), ltaskidx);
					actor::send_actor(aroleid, nguid::make(), pro);
				}
			}
			++ltaskidx;
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

		log_error()->print("activity [{}] rolelevelchange roleid:[{}] beforelevel[{}] nowlevel[{}]", nguid(activityid()), aroleid, abeforelevel, anowlevel);

		auto itor = std::ranges::find_if(lpactivity->m_ranktype, [](int atype)
			{
				return atype == pbdb::eranklist::activity_lv;
			});
		if (itor == lpactivity->m_ranktype.end())
		{
			return;
		}

		// Activityduringgeneratelevelchange
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

		log_error()->print("activity [{}] rolegoldchange roleid:[{}] beforegold[{}] nowgold[{}]", nguid(activityid()), aroleid, abeforegold, anowgold);

		auto itor = std::ranges::find_if(lpactivity->m_ranktype, [](int atype)
			{
				return atype == pbdb::eranklist::activity_gold;
			});
		if (itor == lpactivity->m_ranktype.end())
		{
			return;
		}
		// Activityduringgeneratelevelchange
		pbdb::db_brief* lpbrief = tdb_brief::nsp_cwrite<actor_activity_manage>::instance(actor_activity_manage::actorid()).get(aroleid);
		if (lpbrief != nullptr)
		{
			(*lpbrief->mutable_mactivityvalues()->mutable_mactivity_rolegold())[lpactivity->m_id] += anowgold - abeforegold;
		}
		brief_activityvalues(aroleid);
	}

	void activity::brief_activityvalues([[maybe_unused]] i64_actorid aroleid)
	{
		tdb_brief::nsp_cwrite<actor_activity_manage>::instance(actor_activity_manage::actorid()).change();
	}
}//namespace ngl
