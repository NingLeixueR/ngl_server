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
// File overview: Declares interfaces for actor activity manage.

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

		// # Getactivitytype
		EActivity type()
		{
			return m_tab->m_type;
		}

		// # GetactivityID
		int64_t activityid()
		{
			return m_actorid;
		}

		// # Activitywhetherstart
		virtual bool is_start()
		{
			const pbdb::db_activitytimes* lpdata = m_activitytimes->getconst();
			if (lpdata == nullptr)
			{
				return false;
			}
			return lpdata->mstart();
		}

		// # Activitystarttime
		int32_t start_utc()
		{
			const pbdb::db_activitytimes* lpdata = m_activitytimes->getconst();
			if (lpdata == nullptr)
			{
				return -1;
			}
			return lpdata->mbeg();
		}
		
		// # Activityendtime
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

		// # This activity
		int32_t day()
		{
			return (int32_t)tools::time::getspandays(tools::time::gettime(), start_utc());
		}

		// # Getactivitytable
		const tab_activity* tab()
		{
			return m_tab;
		}

		// # Whethersupportrank list
		bool is_rank()
		{
			return activityid() >= pbdb::eranklist::activity_lv;
		}
	public:
		// #:Activitystart
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

		// #:Activitystartafter server
		virtual void init() 
		{
			log_error()->print("activity::init() activityid=[{}]", activityid());
		}

		// #:Player
		virtual void rolelogin(i64_actorid aroleid);

		// #:Playerstartactivitysuccessful
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

		// #:Playerlevel change
		virtual void rolelevelchange(i64_actorid aroleid, int32_t abeforelevel, int32_t anowlevel);

		// #:Player change
		virtual void rolegoldchange(i64_actorid aroleid, int32_t abeforegold, int32_t anowgold);

		// # Db_brief.m_activityvalues changeafter
		static void brief_activityvalues(i64_actorid aroleid);

		// #:Activityclose
		virtual void finish() 
		{
			log_error()->print("activity::finish() activityid=[{}]", activityid());

			m_activitydb->erase(activityid());
			m_activitytimedb->erase(activityid());
		}
	};
}// namespace ngl
