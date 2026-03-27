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
// File overview: Declares interfaces for actor base.

#pragma once

#include "tools/serialize/nserialize.h"
#include "actor/protocol/nprotocol.h"
#include "tools/serialize/ndefine.h"
#include "tools/serialize/pack.h"
#include "tools/time_wheel.h"

#include <memory>

namespace ngl
{
	class make_timerparm
	{
		make_timerparm() = delete;
		make_timerparm(const make_timerparm&) = delete;
		make_timerparm& operator=(const make_timerparm&) = delete;
	public:
		static time_t month_ms(time_t anow, int amonthday/*1-31*/, int ahour, int amin, int asec)
		{
			std::pair<bool, time_t> lpair = tools::localtime::getmothday(anow, amonthday, ahour, amin, asec);
			if (lpair.first)
			{
				return (lpair.second - anow) * tools::localtime::MILLISECOND;
			}
			return -1;
		}

		// Build a monthly timer that fires on the requested day-of-month at the
		// requested wall-clock time.
		static bool parm_month(np_timerparm& aparm, int amonthday/*1-31*/, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (tools::localtime::check_monthday(amonthday) && tools::localtime::check_hour(ahour) && tools::localtime::check_minute(amin) && tools::localtime::check_sec(asec))
			{
				aparm.m_type = E_ACTOR_TIMER::ET_WEEK;
				aparm.m_ms = month_ms(tools::localtime::gettime(), amonthday, ahour, amin, asec);
				aparm.m_intervalms = [amonthday, ahour, amin, asec](int64_t ams)
					{
						return (int32_t)make_timerparm::month_ms(ams / 1000 + 1, amonthday, ahour, amin, asec);
					};
				aparm.m_count = acount;
				return true;
			}
			return false;			
		}

		// Build a weekly timer that fires on the requested weekday at the
		// requested wall-clock time.
		static bool parm_week(np_timerparm& aparm, int aweek/*1-7*/, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (tools::localtime::check_week(aweek) && tools::localtime::check_hour(ahour) && tools::localtime::check_minute(amin) && tools::localtime::check_sec(asec))
			{
				aparm.m_type = E_ACTOR_TIMER::ET_WEEK;
				time_t lnow = tools::localtime::gettime();
				time_t lfirst = tools::localtime::getweekday(aweek >= 7 ? 0 : aweek, ahour, amin, asec);
				aparm.m_ms = (lfirst - lnow) * tools::localtime::MILLISECOND;
				aparm.m_intervalms = [](int64_t)
					{
						return tools::localtime::WEEK_MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}

		// Build a daily timer that repeats at the requested hour/minute/second.
		static bool parm_day(np_timerparm& aparm, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (tools::localtime::check_hour(ahour) && tools::localtime::check_minute(amin) && tools::localtime::check_sec(asec))
			{
				aparm.m_type = E_ACTOR_TIMER::ET_DAY;
				time_t lnow = tools::localtime::gettime();
				time_t lfirst = tools::localtime::getsecond2time(ahour, amin, asec);
				aparm.m_ms = (lfirst - lnow) * tools::localtime::MILLISECOND;
				aparm.m_intervalms = [](int64_t)
					{
						return tools::localtime::DAY_MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}

		// Build an hourly timer that fires once per hour at minute/second.
		static bool parm_hour(np_timerparm& aparm, int amin, int asec, int acount = 0x7fffffff)
		{
			if (tools::localtime::check_minute(amin) && tools::localtime::check_sec(asec))
			{
				aparm.m_type = E_ACTOR_TIMER::ET_HOUR;
				time_t lnow = tools::localtime::gettime();
				time_t lfirst = tools::localtime::getsecond2time(amin, asec);
				aparm.m_ms = (lfirst - lnow) * tools::localtime::MILLISECOND;
				aparm.m_intervalms = [](int64_t)
					{
						return tools::localtime::HOUR_MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}

		// Build a per-minute timer that fires once per minute at second `asec`.
		static bool parm_min(np_timerparm& aparm, int asec, int acount = 0x7fffffff)
		{
			if (tools::localtime::check_sec(asec))
			{
				aparm.m_type = E_ACTOR_TIMER::ET_MIN;
				time_t lnow = tools::localtime::gettime();
				time_t lfirst = tools::localtime::getsecond2time(asec);
				aparm.m_ms = (lfirst - lnow) * tools::localtime::MILLISECOND;
				aparm.m_intervalms = [](int64_t)
					{
						return tools::localtime::MINUTES_MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}

		// Build a fixed-interval timer measured in seconds.
		static bool make_interval(np_timerparm& aparm, int asec, int acount = 0x7fffffff)
		{
			if (asec >= 0)
			{
				aparm.m_type = E_ACTOR_TIMER::ET_INTERVAL_SEC;
				aparm.m_ms = asec * tools::localtime::MILLISECOND;
				aparm.m_intervalms = [asec](int64_t)
					{
						return asec * tools::localtime::MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}
	};

	class actor_base;

	class ntimer
	{
		ntimer() = delete;
		ntimer(const ntimer&) = delete;
		ntimer& operator=(const ntimer&) = delete;
	public:
		// Register one actor-owned timer and route expirations back as actor messages.
		static int addtimer(actor_base* actor, const std::shared_ptr<np_timerparm>& aparm);
	};
}//namespace ngl
