#pragma once

#include "time_wheel.h"
#include "csvtable.h"
#include "net.pb.h"

namespace ngl
{
	extern time_wheel calendar_wheel;

	class calendar_function
	{
		calendar_function(const calendar_function&) = delete;
		calendar_function& operator=(const calendar_function&) = delete;

		using typecallback = std::function<void(tab_calendar*, int64_t)>;
		struct callback
		{
			typecallback m_start;
			typecallback m_finish;
		};
		static std::map<ECalendarType, callback> m_fun;
	public:
		static void init();

		static void start(tab_calendar* tab, int64_t atime, pbdb::db_calendar* acalendar)
		{
			acalendar->set_m_time(atime);
			auto itor = m_fun.find(tab->m_carendar);
			if (itor == m_fun.end())
			{
				return;
			}
			itor->second.m_start(tab, atime);
			acalendar->set_m_start(true);
		}

		static void finish(tab_calendar* tab, int64_t atime, pbdb::db_calendar* acalendar)
		{
			auto itor = m_fun.find(tab->m_carendar);
			if (itor == m_fun.end())
			{
				return;
			}
			itor->second.m_finish(tab, atime);
			acalendar->set_m_finish(true);
		}

	private:
		static void operator_activ(tab_calendar* tab, int64_t atime, bool astart);
		static void operator_task(tab_calendar* tab, int64_t atime, bool astart);
	};
}//namespace ngl