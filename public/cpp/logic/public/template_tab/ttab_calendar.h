#pragma once

#include "manage_csv.h"
#include "localtime.h"
#include "splite.h"
#include "nlog.h"

namespace ngl
{
	struct ttab_calendar : public manage_csv<tab_calendar>
	{
		static const int m_count = 10;
		struct data
		{
			tab_calendar* m_tab;

			void add(int32_t abeg, int32_t aend)
			{
				m_utc.insert(time(abeg, aend));
			}

			static int64_t time(int32_t abeg, int32_t aend)
			{
				int32_t lvalue[2];
				lvalue[0] = abeg;
				lvalue[1] = aend;
				return *(int64_t*)lvalue;
			}

			static int32_t beg(int64_t atime)
			{
				int32_t lvalue[2];
				*(int64_t*)lvalue = atime;
				return lvalue[0];
			}

			static int32_t end(int64_t atime)
			{
				int32_t lvalue[2];
				*(int64_t*)lvalue = atime;
				return lvalue[1];
			}

			// week
			std::set<int64_t> m_utc;	//key:m_time	
		};

		static std::map<int32_t, data> m_data;

		static bool reload_calendar(int32_t acalendar)
		{
			m_data[acalendar].m_utc.clear();
			auto ltab = tab(acalendar);
			m_data[acalendar].m_tab = ltab;
			if (ltab->m_type == ECalendar::Week)
			{
				init_week(ltab->m_id, ltab->m_week);
			}
			else if (ltab->m_type == ECalendar::ServerOpen)
			{
				init_serveropen(ltab->m_id, ltab->m_serveropen);
			}
			else if (ltab->m_type == ECalendar::RegularSlot)
			{
				init_tregularslot(ltab->m_id, ltab->m_tregularslot);
			}
			return m_data[acalendar].m_utc.empty() != true;
		}

		static int64_t net_calendar(int32_t acalendar, int64_t atime)
		{
			auto itor = m_data.find(acalendar);
			if (itor == m_data.end())
				return -1;
			if (atime == -1)
			{
				if (itor->second.m_utc.empty())
					return -1;
				else
				{
					return *itor->second.m_utc.begin();
				}
			}
			auto itorutc = itor->second.m_utc.find(atime);
			if (itorutc == itor->second.m_utc.end())
			{
				if (reload_calendar(acalendar) == false)
				{
					return -1;
				}
				else
				{
					return net_calendar(acalendar, -1);
				}
			}
			
			return *itorutc;
		}

		ttab_calendar()
		{}

		static tab_calendar* tab(int32_t aid)
		{
			ttab_calendar* ttab = allcsv::get<ttab_calendar>();
			assert(ttab != nullptr);
			auto itor = ttab->tablecsv.find(aid);
			return &itor->second;
		}

		static void post(tab_calendar* atab, int64_t autc, pbdb::db_calendar& acalendar);

		// 获取开服时间utc
		static int32_t serveropen()
		{
			const char* lpopenserver = "2023/10/11 10:00:00";
			return localtime::st2time(lpopenserver);
		}

		// 今日流失的秒数
		static std::pair<bool,int32_t> daysecond(const char* astr)
		{
			//HH:mm:ss
			int hour = 0;
			int minute = 0;
			int second = 0;
			splite::division(astr, ":", hour, minute, second);
			if (hour == -1 || minute == -1 || second == -1)
				return std::make_pair(false,-1);
			return std::make_pair(true, hour * 60 * 60 + minute * 60 + second);
		}

		enum em
		{
			day_utc = 24 * 60 * 60,
			week_utc = 7 * day_utc,
		};

		static void printf_time(int aid)
		{
			std::stringstream m_stream;
			m_stream << "\n";
			m_stream << aid <<":{";
			for (int64_t ltime : m_data[aid].m_utc)
			{
				m_stream 
					<< "	[" 
					<< localtime::time2msstr(data::beg(ltime), "%y/%m/%d %H:%M:%S")
					<< "->" 
					<< localtime::time2msstr(data::end(ltime), "%y/%m/%d %H:%M:%S")
					<< "]\n";
			}
			m_stream << "}\n";
			LogLocalError("%", m_stream.str());
		}

		static void init_week(int aid, std::vector<tweek>& aweek)
		{
			int32_t lnow = localtime::gettime();
			std::vector<int32_t> lvec;
			int32_t lid = 0;
			for (auto& item : aweek)
			{
				int lweekdaystart = localtime::getweekday(item.m_weekstart == 7?0: item.m_weekstart, 0, 0, 0);
				int lweekdayfinish = localtime::getweekday(item.m_weekfinish == 7 ? 0 : item.m_weekfinish, 0, 0, 0);

				std::pair<bool, int32_t> lpairopen = daysecond(item.m_opentime.c_str());
				std::pair<bool, int32_t> lpairclose = daysecond(item.m_closetime.c_str());
				if (lpairclose.first && lpairopen.first)
				{
					assert(lpairclose.second > lpairopen.second);
					int lbeg = lweekdaystart + lpairopen.second;
					int lend = lweekdayfinish + lpairopen.second;
					assert(lend > lbeg);
					if (lnow > lend)
					{
						lbeg += week_utc;
						lend += week_utc;
					}
					m_data[aid].add(lbeg, lend);
				}
			}
			printf_time(aid);
		}

		static void init_serveropen(int aid, std::vector<tserveropen>& atserveropen)
		{
			int32_t lnow = localtime::gettime();
			int32_t lopentime = serveropen();
			int32_t lid = 0;
			for (auto& item : atserveropen)
			{
				// 开启时间
				int32_t lbeg = lopentime + ((item.m_beg - 1) * day_utc);
				std::pair<bool, int32_t> lpairopen = daysecond(item.m_opentime.c_str());
				assert(lpairopen.first);
				lbeg += lpairopen.second;
				// 关闭时间
				int32_t lend = lopentime + ((item.m_end - 1) * day_utc);
				std::pair<bool, int32_t> lpairclose = daysecond(item.m_closetime.c_str());
				assert(lpairclose.first);
				lend += lpairclose.second;
				if (lnow > lend)
				{
					continue;
				}
				m_data[aid].add(lbeg, lend);
			}

			printf_time(aid);
		}

		static void init_tregularslot(int aid, std::vector<tregularslot>& atregularslot)
		{
			int32_t lnow = localtime::gettime();
			for (auto& item : atregularslot)
			{
				int32_t lbeg = localtime::st2time(item.m_opentime.c_str());
				int32_t lclose = localtime::st2time(item.m_closetime.c_str());
				if (lnow > lclose)
					continue;
				m_data[aid].add(lbeg, lclose);
			}
			printf_time(aid);
		}

		virtual void reload()
		{
			//std::map<int, tab_calendar> tablecsv;
			for (std::pair<const int, tab_calendar>& ipair : tablecsv)
			{
				tab_calendar& tab = ipair.second;
				m_data[tab.m_id].m_tab = &tab;
				if (tab.m_type == ECalendar::Week)
				{
					init_week(tab.m_id, tab.m_week);
				}
				else if (tab.m_type == ECalendar::ServerOpen)
				{
					init_serveropen(tab.m_id, tab.m_serveropen);
				}
				else if (tab.m_type == ECalendar::RegularSlot)
				{
					init_tregularslot(tab.m_id, tab.m_tregularslot);
				}
			}
		}

		static bool is_period(int aid)
		{
			data* lpdata = tools::findmap(m_data, aid);
			if (lpdata == nullptr)
				return false;
			int32_t lnow = localtime::gettime();
			for (int64_t item : lpdata->m_utc)
			{
				if (lnow >= data::beg(item) && lnow <= data::end(item))
					return true;
			}
			return false;
		}
	};
}// namespace ngl