#pragma once

#include "manage_csv.h"
#include "splite.h"
#include "localtime.h"
#include "nlog.h"

namespace ngl
{
	struct ttab_calendar : public manage_csv<tab_calendar>
	{
		static const int m_count = 10;
		struct data
		{
			tab_calendar* m_tab;
			struct begend_utc
			{
				int32_t m_beg;
				int32_t m_end;
			};
			// week
			std::vector<begend_utc> m_utc;			
		};

		static std::map<int32_t, data> m_data;

		ttab_calendar()
		{}

		// 获取开服时间utc
		int32_t serveropen()
		{
			const char* lpopenserver = "2023/10/11 10:00:00";
			return localtime::st2time(lpopenserver);
		}

		// 今日流失的秒数
		std::pair<bool,int32_t> daysecond(const char* astr)
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

		void printf_time(int aid)
		{
			std::string lbeg = localtime::time2msstr(m_data[aid].m_utc.rbegin()->m_beg, "%y/%m/%d %H:%M:%S");
			std::string lend = localtime::time2msstr(m_data[aid].m_utc.rbegin()->m_beg, "%y/%m/%d %H:%M:%S");
			LogLocalInfo("tab_calendar[%][%->%]", aid, lbeg, lend);
		}

		// 
		void init_week(int aid, std::vector<tweek>& aweek)
		{
			std::vector<int32_t> lvec;
			for (auto& item : aweek)
			{
				int lweekday = localtime::getweekday(item.m_week == 7?0: item.m_week, 0, 0, 0);

				std::pair<bool, int32_t> lpairopen = daysecond(item.m_opentime.c_str());
				std::pair<bool, int32_t> lpairclose = daysecond(item.m_closetime.c_str());
				if (lpairclose.first && lpairopen.first)
				{
					assert(lpairclose.second > lpairopen.second);
				}
				
				data::begend_utc lbegend;
				if (lpairopen.first)
					lvec.push_back(lweekday + lpairopen.second);
				else
					lvec.push_back(-1);

				if (lpairclose.first)
					lvec.push_back(lweekday + lpairclose.second);
				else
					lvec.push_back(-1);
			}
			std::vector<int32_t> lweek;
			// 整理
			for (int i = 0; i < lvec.size(); ++i)
			{
				if (lvec[i] == -1)
					continue;
				lweek.push_back(lvec[i]);
				// 有效数字应该保持两个数组奇偶对应
				// 比如1代表有效-1代表无效
				// 例子1 {1,-1,1,1,1,1}   第二个有效数字(偶数位置)却是奇数位置 所有这个数列是有问题的
				assert(lweek.size() % 2 == (i + 1) % 2);
			}

			// 产生m_count组数据
			for (int i = 0; i < m_count; ++i)
			{
				for (int j = 0; j < lweek.size(); j += 2)
				{
					m_data[aid].m_utc.push_back(data::begend_utc
						{
							.m_beg = lweek[j] + i * week_utc,
							.m_end = lweek[j + 1] + i * week_utc,
						});
					printf_time(aid);
				}
			}
		}

		void init_serveropen(int aid, std::vector<tserveropen>& atserveropen)
		{
			int32_t lopentime = serveropen();
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
				m_data[aid].m_utc.push_back(data::begend_utc
					{
						.m_beg = lbeg,
						.m_end = lend,
					});

				printf_time(aid);
			}
		}

		void init_tregularslot(int aid, std::vector<tregularslot>& atregularslot)
		{
			for (auto& item : atregularslot)
			{
				int32_t lbeg = localtime::st2time(item.m_opentime.c_str());
				int32_t lclose = localtime::st2time(item.m_closetime.c_str());
				m_data[aid].m_utc.push_back(data::begend_utc
					{
						.m_beg = lbeg,
						.m_end = lclose,
					});

				printf_time(aid);
			}
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
			for (auto& item : lpdata->m_utc)
			{
				if (lnow >= item.m_beg && lnow <= item.m_end)
					return true;
			}
			return false;
		}

		static bool get(
			int aid
			, int alastendutc // 上次结束时候的utc
			, int32_t& abeg
			, int32_t& aend)
		{
			data* lpdata = tools::findmap(m_data, aid);
			if (lpdata == nullptr)
				return false;
			int32_t lnow = localtime::gettime();
			for (auto& item : lpdata->m_utc)
			{
				if (lnow <= item.m_end && item.m_beg > alastendutc)
				{
					abeg = item.m_beg;
					aend = item.m_end;
					return true;
				}
			}
			return false;
		}
	};
}