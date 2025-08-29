#pragma once

#include "time_wheel.h"
#include "nserialize.h"
#include "nprotocol.h"
#include "ndefine.h"
#include "pack.h"

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
			std::pair<bool, time_t> lpair = localtime::getmothday(anow, amonthday, ahour, amin, asec);
			if (lpair.first)
			{
				return (lpair.second - anow) * localtime::MILLISECOND;
			}
			return -1;
		}

		//ET_MONTH,		// 每月触发
		static bool month(np_timerparm& aparm, int amonthday/*1-31*/, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (
				localtime::check_monthday(amonthday) 
				&& localtime::check_hour(ahour) 
				&& localtime::check_minute(amin) 
				&& localtime::check_sec(asec)
				)
			{
				aparm.m_type = E_ACTOR_TIMER::ET_WEEK;
				aparm.m_ms = month_ms(localtime::gettime(), amonthday, ahour, amin, asec);
				aparm.m_intervalms = [amonthday, ahour, amin, asec](int64_t ams)
					{
						return (int32_t)make_timerparm::month_ms(ams / 1000 + 1, amonthday, ahour, amin, asec);
					};
				aparm.m_count = acount;
				return true;
			}
			return false;			
		}

		// 每周触发 1-7
		static bool week(np_timerparm& aparm, int aweek/*1-7*/, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (
				localtime::check_week(aweek) 
				&& localtime::check_hour(ahour) 
				&& localtime::check_minute(amin) 
				&& localtime::check_sec(asec)
				)
			{
				aparm.m_type = E_ACTOR_TIMER::ET_WEEK;
				time_t lnow = localtime::gettime();
				time_t lfirst = localtime::getweekday(aweek >= 7 ? 0 : aweek, ahour, amin, asec);
				aparm.m_ms = (lfirst - lnow) * localtime::MILLISECOND;
				aparm.m_intervalms = [](int64_t)
					{
						return localtime::WEEK_MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}

		// 每日触发  ahour时amin分asec秒
		static bool day(np_timerparm& aparm, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (
				localtime::check_hour(ahour) 
				&& localtime::check_minute(amin) 
				&& localtime::check_sec(asec)
				)
			{
				aparm.m_type = E_ACTOR_TIMER::ET_DAY;
				time_t lnow = localtime::gettime();
				time_t lfirst = localtime::getsecond2time(ahour, amin, asec);
				aparm.m_ms = (lfirst - lnow) * localtime::MILLISECOND;
				aparm.m_intervalms = [](int64_t)
					{
						return localtime::DAY_MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}

		// 每小时触发  amin分asec秒
		static bool hour(np_timerparm& aparm, int amin, int asec, int acount = 0x7fffffff)
		{
			if (localtime::check_minute(amin) && localtime::check_sec(asec))
			{
				aparm.m_type = E_ACTOR_TIMER::ET_HOUR;
				time_t lnow = localtime::gettime();
				time_t lfirst = localtime::getsecond2time(amin, asec);
				aparm.m_ms = (lfirst - lnow) * localtime::MILLISECOND;
				aparm.m_intervalms = [](int64_t)
					{
						return localtime::HOUR_MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}

		// 每分钟触发  asec秒
		static bool min(np_timerparm& aparm, int asec, int acount = 0x7fffffff)
		{
			if (localtime::check_sec(asec))
			{
				aparm.m_type = E_ACTOR_TIMER::ET_MIN;
				time_t lnow = localtime::gettime();
				time_t lfirst = localtime::getsecond2time(asec);
				aparm.m_ms = (lfirst - lnow) * localtime::MILLISECOND;
				aparm.m_intervalms = [](int64_t)
					{
						return localtime::MINUTES_MILLISECOND;
					};
				aparm.m_count = acount;
				return true;
			}
			return false;
		}

		// 每n秒触发
		static bool make_interval(np_timerparm& aparm, int asec, int acount = 0x7fffffff)
		{
			if (asec >= 0)
			{
				aparm.m_type = E_ACTOR_TIMER::ET_INTERVAL_SEC;
				aparm.m_ms = asec * localtime::MILLISECOND;
				aparm.m_intervalms = [asec](int64_t)
					{
						return asec * localtime::MILLISECOND;
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
		// 抛出一个定时器
		static int addtimer(actor_base* actor, const std::shared_ptr<np_timerparm>& aparm);
	};
}//namespace ngl
