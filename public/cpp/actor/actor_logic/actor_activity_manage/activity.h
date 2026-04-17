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
// File overview: Base activity class defining the common interface for all activity types.

#pragma once

#include "actor/actor_logic/actor_activity_manage/activitytimedb.h"
#include "actor/actor_logic/actor_activity_manage/activitydb.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/tab/ttab_activity.h"

namespace ngl
{
	class activity
	{
		activity(const activity&) = delete;
		activity& operator=(const activity&) = delete;

		static std::map<EActivity, activity*> m_activityall;
		virtual std::shared_ptr<activity> create(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			return std::make_shared<activity>(aactivityid, atime, aduration, aactivitydb, aactivitytimedb);
		}
	public:
		static std::shared_ptr<activity> make(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			const tab_activity* ltab = ttab_activity::instance().tab(aactivityid);
			auto itor = m_activityall.find(ltab->m_type);
			if (itor == m_activityall.end())
			{
				return nullptr;
			}
			return itor->second->create(aactivityid, atime, aduration, aactivitydb, aactivitytimedb);
		}
	protected:
		i64_actorid m_actorid									= 0;
		const tab_activity* m_tab								= nullptr;
		data_modified<pbdb::db_activity>* m_activity			= nullptr;
		data_modified<pbdb::db_activitytimes>* m_activitytimes	= nullptr;
		activitydb* m_activitydb								= nullptr;
		activitytimedb* m_activitytimedb						= nullptr;
	public:
		activity(int32_t activityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb);

		activity(EActivity atype);

		// Get activity type
		EActivity type()
		{
			return m_tab->m_type;
		}

		// Get activity ID
		int64_t activityid()
		{
			return m_actorid;
		}

		// Check whether the activity has started
		virtual bool is_start()
		{
			const pbdb::db_activitytimes* lpdata = m_activitytimes->getconst();
			if (lpdata == nullptr)
			{
				return false;
			}
			return lpdata->mstart();
		}

		// Get activity start time (UTC)
		int32_t start_utc()
		{
			const pbdb::db_activitytimes* lpdata = m_activitytimes->getconst();
			if (lpdata == nullptr)
			{
				return -1;
			}
			return lpdata->mbeg();
		}
		
		// Get activity end time (UTC)
		int32_t finish_utc()
		{
			const pbdb::db_activitytimes* lpdata = m_activitytimes->getconst();
			if (lpdata == nullptr)
			{
				return -1;
			}
			if (lpdata->mduration() == -1)
			{
				return -1;
			}
			return start_utc() + lpdata->mduration();
		}

		// Get the number of days since this activity started
		int32_t day()
		{
			return (int32_t)tools::time::getspandays(tools::time::gettime(), start_utc());
		}

		// Get the activity config table entry
		const tab_activity* tab()
		{
			return m_tab;
		}

		// Check whether this activity supports a rank list
		bool is_rank()
		{
			return activityid() >= pbdb::eranklist::activity_lv;
		}
	public:
		// Called when the activity starts
		virtual void start()
		{
			const pbdb::db_activitytimes* lpconstdata = m_activitytimes->getconst();
			if (lpconstdata == nullptr)
			{
				return;
			}
			if (lpconstdata->mstart())
			{
				return;
			}
			log_error()->print("activity::start() activityid=[{}]", activityid());
			pbdb::db_activitytimes* lpdata = m_activitytimes->get();
			if (lpdata == nullptr)
			{
				return;
			}
			lpdata->set_mstart(true);
		}

		// Called after server startup to initialize the activity
		virtual void init() 
		{
			log_error()->print("activity::init() activityid=[{}]", activityid());
		}

		// Called when a player logs in during this activity
		virtual void rolelogin(i64_actorid aroleid);

		// Handle a player's activity task response (accept or complete)
		void recv_task_response(i64_actorid aroleid, int32_t aindex, bool aisreceive)
		{
			pbdb::db_activity* lpdata = m_activity->get();
			if (lpdata == nullptr)
			{
				return;
			}
			auto* ltaskmap = lpdata->mutable_mtask();
			pbdb::activity_task& ltask = (*ltaskmap)[aroleid];
			if (aisreceive)
			{
				auto* lopen = ltask.mutable_mopen();
				(*lopen)[aindex] = true;
			}
			else
			{
				auto* lclose = ltask.mutable_mclose();
				(*lclose)[aindex] = true;
			}
		}

		// Called when a player's level changes
		virtual void rolelevelchange(i64_actorid aroleid, int32_t abeforelevel, int32_t anowlevel);

		// Called when a player's gold changes
		virtual void rolegoldchange(i64_actorid aroleid, int32_t abeforegold, int32_t anowgold);

		// Notify after db_brief.m_activityvalues changes
		static void brief_activityvalues(i64_actorid aroleid);

		// Called when the activity ends
		virtual void finish() 
		{
			log_error()->print("activity::finish() activityid=[{}]", activityid());

			m_activitydb->erase(activityid());
			m_activitytimedb->erase(activityid());
		}
	};
}// namespace ngl
