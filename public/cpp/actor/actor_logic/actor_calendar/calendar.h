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

	class calendar : public tdb_calendar::db_modular
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

		void next_calendar(int64_t acalendarid, int64_t atime)
		{
			std::map<nguid, data_modified<pbdb::db_calendar>>* lmap = get_calendar();
			auto itor = lmap->find(acalendarid);
			if (itor == lmap->end())
			{
				return;
			}
			pbdb::db_calendar& lcalendar = itor->second.get();
			int64_t ltime = ttab_calendar::net_calendar((int32_t)acalendarid, atime);
			if (ltime <= 0)
			{
				return;
			}
			lcalendar.set_m_id(acalendarid);
			lcalendar.set_m_time(ltime);
			lcalendar.set_m_finish(false);
			lcalendar.set_m_start(false);
			ttab_calendar::post(ttab_calendar::tab((int32_t)acalendarid), ltime, lcalendar);
		}

		pbdb::db_calendar* get_calendar(int64_t acalendarid, bool achange = true)
		{
			std::map<nguid, data_modified<pbdb::db_calendar>>* lmap = get_calendar();
			auto itor = lmap->find(acalendarid);
			if (itor == lmap->end())
			{
				return nullptr;
			}
			return &itor->second.get(achange);
		}


		pbdb::db_calendar const* get_constcalendar(int64_t acalendarid)
		{
			std::map<nguid, data_modified<pbdb::db_calendar>>* lmap = get_calendar();
			auto itor = lmap->find(acalendarid);
			if (itor == lmap->end())
			{
				return nullptr;
			}
			return &itor->second.getconst();
		}

		virtual void initdata()
		{		
			std::map<nguid, data_modified<pbdb::db_calendar>>* lmap = get_calendar();

			log_error()->print("actor_calendar###loaddb_finish");
			int32_t lnow = (int32_t)localtime::gettime();
			for (std::pair<const nguid, data_modified<pbdb::db_calendar> >& item : *lmap)
			{
				const pbdb::db_calendar& lcalendar = item.second.getconst();

				int64_t ltime = lcalendar.m_time();
				int32_t lbeg = ttab_calendar::data::beg(ltime);
				int32_t lend = ttab_calendar::data::end(ltime);
				
				log_error()->print(
					"calendar[{}] start[{}:{}]finish[{}:{}]", 
					item.first.id(),
					localtime::time2str(lbeg, "%y/%m/%d %H:%M:%S"), 
					lcalendar.m_start() ? 1 : 0,
					localtime::time2str(lend, "%y/%m/%d %H:%M:%S"),
					lcalendar.m_finish() ? 1 : 0
				);

				pbdb::db_calendar* itemcalendar = get_calendar(item.first.id());
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
					calendar_function::trigger(tab, ltime, true, itemcalendar);
				}
				else
				{
					ttab_calendar::post(tab, ltime, *itemcalendar);
				}
				if (lcalendar.m_finish() == false && lnow >= lend)
				{
					// 执行 finish
					calendar_function::trigger(tab, ltime, false, itemcalendar);
				}
				else
				{
					ttab_calendar::post(tab, ltime, *itemcalendar);
				}
			}
			// ##
			for (std::pair<const int32_t, ttab_calendar::data>& item :ttab_calendar::m_data)
			{
				if (get_constcalendar(item.first) == nullptr)
				{
					pbdb::db_calendar lcalendar;
					int64_t ltime = ttab_calendar::net_calendar(item.first, -1);
					if (ltime <= 0)
					{
						continue;
					}
					lcalendar.set_m_id(item.first);
					lcalendar.set_m_time(ltime);
					lcalendar.set_m_finish(false);
					lcalendar.set_m_start(false);
					add(item.first, lcalendar);
					ttab_calendar::post(ttab_calendar::tab(item.first), ltime, *get_calendar(item.first));
				}
			}
		}

		// # 添加历史触发列表
		void add_trigger_list(int64_t acalendarid, int32_t atriggertime, bool aisstart)
		{
			auto ldatda = get_calendar();
			auto itor = ldatda->find(acalendarid);
			if (itor == ldatda->end())
			{
				return;
			}
			auto ltrigger = itor->second.get().add_m_triggerlist();
			ltrigger->set_m_triggertime(atriggertime);
			ltrigger->set_m_isstart(aisstart);

			auto lptriggerlist = itor->second.get().mutable_m_triggerlist();
			if (lptriggerlist == nullptr)
			{
				return;
			}
			int32_t l2week = localtime::gettime() - 2*localtime::WEEK_SECOND;
			auto itors = lptriggerlist->begin();
			for (; itors != lptriggerlist->end();)
			{
				if (l2week < itors->m_triggertime())
				{
					itors = lptriggerlist->erase(itors);
				}
				else
				{
					++itors;
				}
			}
			
		}

		// # 获取历史触发列表
		void get_trigger_list(int32_t atriggertime, std::map<int64_t, std::vector<calendar_trigger>>& acalendarlist)
		{
			for (const auto& item1 : *get_calendar())
			{
				int64_t calendarid = item1.first;
				for (const auto& item2 : item1.second.getconst().m_triggerlist())
				{
					if (atriggertime < item2.m_triggertime())
					{
						acalendarlist[calendarid].push_back(calendar_trigger
							{
								.m_triggerutc = item2.m_triggertime(),
								.m_isstart = item2.m_isstart()
							});
					}
				}
			}
		}
	};
}// namespace ngl