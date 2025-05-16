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

		using tcall = std::tuple<
			std::function<bool(tab_calendar*, pbdb::db_calendar&, int32_t, i64_actorid)>,
			std::function<bool(tab_calendar*, pbdb::db_calendar&, int32_t, i64_actorid)>
		>;
		static std::array<tcall, ECalendarType::ECalendarTypeCount> m_call;

		static void push(
			ECalendarType atype,
			const std::function<bool(tab_calendar*, pbdb::db_calendar&, int32_t, i64_actorid)>& astart,
			const std::function<bool(tab_calendar*, pbdb::db_calendar&, int32_t, i64_actorid)>& afinish
		);
	public:
		static void init();

		static bool trigger(tab_calendar* tab, pbdb::db_calendar& acalendar, int64_t atime, bool aisstart,  i64_actorid aactor = nguid::make())
		{
			if (aisstart)
			{
				return start(tab, acalendar, atime, aactor);
			}
			else
			{
				return finish(tab, acalendar, atime, aactor);
			}
		}

	private:
		static bool start(tab_calendar* tab, pbdb::db_calendar& acalendar, int32_t atime, i64_actorid aactor)
		{
			if (tab->m_carendar >= ECalendarTypeCount)
			{
				return false;
			}
			acalendar.set_m_start(atime);
			tcall& lcall = m_call[tab->m_carendar];
			auto lstart = std::get<0>(lcall);
			return lstart(tab, acalendar, atime, aactor);
		}

		static bool finish(tab_calendar* tab, pbdb::db_calendar& acalendar, int32_t atime, i64_actorid aactor)
		{
			if (tab->m_carendar >= ECalendarTypeCount)
			{
				return false;
			}
			acalendar.set_m_finish(atime);
			tcall& lcall = m_call[tab->m_carendar];
			auto lfinish = std::get<1>(lcall);
			return lfinish(tab, acalendar, atime, aactor);
		}

		static bool operator_activ(tab_calendar* tab, pbdb::db_calendar& acalendar, int64_t atime, bool astart, i64_actorid aactor);
		static bool operator_task(tab_calendar* tab, pbdb::db_calendar& acalendar, int64_t atime, bool astart, i64_actorid aactor);
	};
}//namespace ngl