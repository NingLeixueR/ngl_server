#pragma once

#include "calendar_function.h"
#include "ttab_calendar.h"
#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	struct calendar_trigger
	{
		int64_t m_triggerutc = -1;
		bool m_isstart = false;
	};

	class calendar : 
		public tdb_calendar::db_modular
	{
		calendar(const calendar&) = delete;
		calendar& operator=(const calendar&) = delete;
	public:
		calendar()
		{}

		virtual void set_id()
		{
			m_id = -1;
		}

		std::map<nguid, data_modified<pbdb::db_calendar>>* get_calendar()
		{
			return &data();
		}

		data_modified<pbdb::db_calendar>* get_calendar(int64_t acalendarid)
		{
			std::map<nguid, data_modified<pbdb::db_calendar>>* lmap = get_calendar();
			auto itor = lmap->find(acalendarid);
			if (itor == lmap->end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void next_calendar(int64_t acalendarid, int64_t atime)
		{
			data_modified<pbdb::db_calendar>* lpcalendar = get_calendar(acalendarid);
			if (lpcalendar == nullptr)
			{
				return;
			}
			pbdb::db_calendar& lcalendar = lpcalendar->get();
			int64_t ltime = ttab_calendar::net_calendar((int32_t)acalendarid, atime);
			if (ltime <= 0)
			{
				return;
			}
			lcalendar.set_m_id(acalendarid);
			lcalendar.set_m_start(ttab_calendar::data::beg(ltime));
			lcalendar.set_m_finish(ttab_calendar::data::end(ltime));
			ttab_calendar::post(ttab_calendar::tab((int32_t)acalendarid), ltime, true);
			ttab_calendar::post(ttab_calendar::tab((int32_t)acalendarid), ltime, false);
		}

		virtual void initdata()
		{
			log_error()->print("actor_calendar###loaddb_finish\n{}", data());
			std::map<nguid, data_modified<pbdb::db_calendar>>* lmap = get_calendar();

			int32_t lnow = (int32_t)localtime::gettime();
			for (std::pair<const nguid, data_modified<pbdb::db_calendar> >& item : *lmap)
			{
				const pbdb::db_calendar& lcalendar = item.second.getconst();

				int32_t lbeg = lcalendar.m_start();
				int32_t lend = lcalendar.m_finish();
				
				data_modified<pbdb::db_calendar>* itemcalendar = get_calendar(item.first.id());
				if (itemcalendar == nullptr)
				{
					continue;
				}
				tab_calendar* tab = ttab_calendar::tab((int32_t)item.first);
				if (tab == nullptr)
				{
					continue;
				}

				if (lcalendar.m_start() == false && lnow >= lbeg)
				{
					// 执行 start
					calendar_function::trigger(tab, itemcalendar->get(), lbeg, true);
				}
				else
				{
					ttab_calendar::post(tab, lbeg, true);
				}
				if (lcalendar.m_finish() == false && lnow >= lend)
				{
					// 执行 finish
					calendar_function::trigger(tab, itemcalendar->get(), lend, false);
				}
				else
				{
					ttab_calendar::post(tab, lend, false);
				}
			}
			
			for (std::pair<const int32_t, ttab_calendar::data>& item :ttab_calendar::m_data)
			{
				if (get_calendar(item.first) == nullptr)
				{
					pbdb::db_calendar lcalendar;
					int64_t ltime = ttab_calendar::net_calendar(item.first, -1);
					if (ltime <= 0)
					{
						continue;
					}
					lcalendar.set_m_id(item.first);
					lcalendar.set_m_start(ttab_calendar::data::beg(ltime));
					lcalendar.set_m_finish(ttab_calendar::data::end(ltime));
					add(item.first, lcalendar);
					ttab_calendar::post(ttab_calendar::tab(item.first), ttab_calendar::data::beg(ltime), true);
					ttab_calendar::post(ttab_calendar::tab(item.first), ttab_calendar::data::beg(ltime), false);
				}
			}
		}

		// # 添加历史触发列表
		void add_trigger_list(int64_t acalendarid, int32_t atriggertime, bool aisstart)
		{
			data_modified<pbdb::db_calendar>* lpcalendar = get_calendar(acalendarid);
			if (lpcalendar == nullptr)
			{
				return;
			}

			auto& lcalendar = lpcalendar->get();
			auto& lptriggerlist = *lcalendar.mutable_m_triggerlist();
			auto& ltriggerlistitem = lptriggerlist[atriggertime];
			ltriggerlistitem.set_m_isstart(aisstart);
			ltriggerlistitem.set_m_triggertime(atriggertime);

			int32_t l2week = localtime::gettime() - 2*localtime::WEEK_SECOND;
			lptriggerlist.erase(lptriggerlist.begin(), std::lower_bound(lptriggerlist.begin(), lptriggerlist.end(), l2week,
				[](const auto& apair, int32_t aval)->bool
				{
					return apair.m_triggertime() < aval;
				}));
		}

		// # 获取actor最后一次触发时间
		int32_t get_actor_last_triggertime(int64_t acalendarid, int32_t aactorid)
		{
			data_modified<pbdb::db_calendar>* lpcalendar = get_calendar(acalendarid);
			if (lpcalendar == nullptr)
			{
				return -1;
			}
			const pbdb::db_calendar& lcalendar = lpcalendar->getconst();
			auto itor = lcalendar.m_actortrigger().find(aactorid);
			if (itor == lcalendar.m_actortrigger().end())
			{
				return -1;
			}
			return itor->second;
		}

		static std::map<ENUM_ACTOR, std::set<ECalendarType>> m_actortypeofcalendartype;

		// # 获取历史触发列表
		void get_trigger_list(int64_t aactorid, std::map<int64_t, std::vector<pbdb::calendar_pair>>& acalendarlist)
		{
			// ECalendarTypeActivity	->		ACTOR_ACTIVITY_MANAGE
			// ECalendarTypeTask		->		ACTOR_ROLE
			ENUM_ACTOR lactortype = (ENUM_ACTOR)nguid::type(aactorid);

			std::set<ECalendarType>* lpset = tools::findmap(m_actortypeofcalendartype, lactortype);
			if (lpset == nullptr)
			{
				log_error()->print("calendar::get_trigger_list fail {}", nguid(aactorid));
				return;
			}

			for (const auto& item1 : *get_calendar())
			{
				int64_t lcalendarid = item1.first;
				tab_calendar* tab = ttab_calendar::tab(lcalendarid);
				if (tab == nullptr)
				{
					continue;
				}
				if (!lpset->contains(tab->m_carendar))
				{
					continue;
				}
				int32_t ltriggertime = get_actor_last_triggertime(lcalendarid, aactorid);
				const auto& triggerlist = item1.second.getconst().m_triggerlist();
				auto itor = std::lower_bound(triggerlist.begin(), triggerlist.end(), ltriggertime,
					[](const auto& apair, int32_t aval)->bool
					{
						return apair.m_triggertime() < aval;
					});
				for (;itor != triggerlist.end();++itor)
				{
					acalendarlist[lcalendarid].push_back(*itor);
				}
			}
		}

		//# 记录actor触发完成
		void add_actor_trigger(i64_actorid aactorid, const ncalendar_info& ainfo)
		{
			data_modified<pbdb::db_calendar>* lpcalendar = get_calendar(ainfo.m_calendarid);
			if (lpcalendar == nullptr)
			{
				return;
			}
			int32_t& lvalue = (*lpcalendar->get().mutable_m_actortrigger())[aactorid];
			lvalue = std::max(lvalue, ainfo.m_time);
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_calendar)