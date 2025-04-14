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

			auto lstream = log_error();
			(*lstream) << "actor_calendar###loaddb_finish" << std::endl;
			int32_t lnow = (int32_t)localtime::gettime();
			for (std::pair<const nguid, data_modified<pbdb::db_calendar> >& item : *lmap)
			{
				(*lstream) << std::format("calendar[{}]", item.first.id()) << std::endl;
				const pbdb::db_calendar& lcalendar = item.second.getconst();


				int64_t ltime = lcalendar.m_time();
				int32_t lbeg = ttab_calendar::data::beg(ltime);
				int32_t lend = ttab_calendar::data::end(ltime);
				
				(*lstream)
					<< std::format(
						"start[{}:{}]",
						localtime::time2str(lbeg, "%y/%m/%d %H:%M:%S"),
						lcalendar.m_start() ? "true" : "false"
					)
					<< std::format(
						"finish[{}:{}]",
						localtime::time2str(lend, "%y/%m/%d %H:%M:%S"),
						lcalendar.m_finish() ? "true" : "false"
					) << std::endl;

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
					// ִ�� start
					calendar_function::start(tab, ltime, itemcalendar);
				}
				else
				{
					ttab_calendar::post(tab, ltime, *itemcalendar);
				}
				if (lcalendar.m_finish() == false && lnow >= lend)
				{
					// ִ�� finish
					calendar_function::finish(tab, ltime, itemcalendar);
				}
				else
				{
					ttab_calendar::post(tab, ltime, *itemcalendar);
				}
			}
			(*lstream).print("");
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
	};
}// namespace ngl