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
			lcalendar.set_m_start(ttab_calendar::data::beg(ltime));
			lcalendar.set_m_finish(ttab_calendar::data::end(ltime));
			ttab_calendar::post(ttab_calendar::tab((int32_t)acalendarid), ltime, true, lcalendar);
			ttab_calendar::post(ttab_calendar::tab((int32_t)acalendarid), ltime, false, lcalendar);
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
			log_error()->print("actor_calendar###loaddb_finish\n{}", data());
			std::map<nguid, data_modified<pbdb::db_calendar>>* lmap = get_calendar();

			int32_t lnow = (int32_t)localtime::gettime();
			for (std::pair<const nguid, data_modified<pbdb::db_calendar> >& item : *lmap)
			{
				const pbdb::db_calendar& lcalendar = item.second.getconst();

				int32_t lbeg = lcalendar.m_start();
				int32_t lend = lcalendar.m_finish();
				
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
					calendar_function::trigger(tab, itemcalendar, lbeg, true);
				}
				else
				{
					ttab_calendar::post(tab, lbeg, true, *itemcalendar);
				}
				if (lcalendar.m_finish() == false && lnow >= lend)
				{
					// 执行 finish
					calendar_function::trigger(tab, itemcalendar, lend, false);
				}
				else
				{
					ttab_calendar::post(tab, lend, false, *itemcalendar);
				}
			}
			
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
					lcalendar.set_m_start(ttab_calendar::data::beg(ltime));
					lcalendar.set_m_finish(ttab_calendar::data::end(ltime));
					add(item.first, lcalendar);
					ttab_calendar::post(ttab_calendar::tab(item.first), ttab_calendar::data::beg(ltime), true, *get_calendar(item.first));
					ttab_calendar::post(ttab_calendar::tab(item.first), ttab_calendar::data::beg(ltime), false, *get_calendar(item.first));
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

			auto& ltemp = itor->second.get();
			auto& lptriggerlist = *ltemp.mutable_m_triggerlist();
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
			auto lpdata = get_constcalendar(acalendarid);
			if (lpdata == nullptr)
			{
				return -1;
			}
			auto itor = lpdata->m_actortrigger().find(aactorid);
			if (itor == lpdata->m_actortrigger().end())
			{
				return -1;
			}
			return itor->second;
		}

		// # 获取历史触发列表
		void get_trigger_list(int64_t aactorid, std::map<int64_t, std::vector<pbdb::calendar_pair>>& acalendarlist)
		{
			// ECalendarTypeActivity	->		ACTOR_ACTIVITY_MANAGE
			// ECalendarTypeTask		->		ACTOR_ROLE
			i16_actortype lactortype = nguid::type(aactorid);
			ECalendarType ltype = ECalendarTypeNull;
			switch (lactortype)
			{
			case ACTOR_ACTIVITY_MANAGE:
				ltype = ECalendarTypeActivity;
				break;
			case ACTOR_ROLE:
				ltype = ECalendarTypeTask;
				break;
			}
			for (const auto& item1 : *get_calendar())
			{
				int64_t lcalendarid = item1.first;
				tab_calendar* tab = ttab_calendar::tab(lcalendarid);
				if (tab == nullptr)
				{
					continue;
				}
				if (tab->m_carendar != ltype)
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
		void add_actor_trigger(int64_t aactorid, const ncalendar_info& ainfo)
		{
			pbdb::db_calendar* lpcalendar = get_calendar(ainfo.m_calendarid);
			if (lpcalendar == nullptr)
			{
				return;
			}
			int32_t& lvalue = (*lpcalendar->mutable_m_actortrigger())[aactorid];
			if (lvalue > ainfo.m_time)
			{
				lvalue = ainfo.m_time;
			}
		}
	};
}// namespace ngl

template <>
struct std::formatter<ngl::data_modified<pbdb::db_calendar>>
{
	constexpr auto parse(const std::format_parse_context& ctx)const
	{
		return ctx.begin();
	}

	auto format(const ngl::data_modified<pbdb::db_calendar>& aval, std::format_context& ctx)const
	{
		const auto& lcalendar = aval.getconst();
		return std::format_to(ctx.out(),"<calendar={},start={}:{},finish={}:{}>\n",
			lcalendar.m_id(), 
			ngl::localtime::time2str(lcalendar.m_start(), "%y/%m/%d %H:%M:%S"), lcalendar.m_isstart() ? 1 : 0,
			ngl::localtime::time2str(lcalendar.m_finish(), "%y/%m/%d %H:%M:%S"), lcalendar.m_isfinish() ? 1 : 0
		);
	}
};