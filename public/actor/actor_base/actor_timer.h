#pragma once

#include <memory>

#include "actor_define.h"
#include "actor_protocol.h"
#include "time_wheel.h"
#include "serialize.h"
#include "pack.h"

namespace ngl
{

	struct timerparm
	{
		enum E_ACTOR_TIMER
		{
			ET_NULL,
			ET_MONTH,		// 每月触发
			ET_WEEK,		// 每周触发
			ET_DAY,			// 每日触发		ahour时amin分asec秒
			ET_HOUR,		// 每小时触发	amin分asec秒
			ET_MIN,			// 每分钟触发	asec秒
			ET_INTERVAL_SEC,// 间隔n秒触发
		};
		int m_type = 0;
		int m_timerid = 0;
		int64_t m_ms = 0;		// 相对于当前时间 ms
		std::function<int(int64_t)> m_intervalms = nullptr;	// 触发间隔
		int m_count = 1;		// 触发次数
		std::shared_ptr<void> m_parm = nullptr;	// 自定义参数
		int64_t m_triggerms = 0;		// 触发时的毫秒

		def_portocol(
			timerparm,
			m_type,
			m_timerid,
			m_ms,
			m_count,
			m_triggerms
		)

			template <typename T>
		void set_parm(T* aparm)
		{
			m_parm = std::shared_ptr<void>(aparm);
		}
	};


	class make_timerparm
	{

	public:
		static time_t month_ms(int anow, int autc, int amonthday = 1/*1-31*/, int ahour = 0, int amin = 0, int asec = 0)
		{
			time_t lfirst = localtime::getmothday(autc, amonthday, ahour, amin, asec);
			char lbuff[1024] = { 0 };
			ngl::localtime::time2str(lbuff, 1024, lfirst, "%y/%m/%d %H:%M:%S");
			std::cout << lbuff << std::endl;
			return (lfirst - anow) * localtime::MILLISECOND;
		}

		//ET_MONTH,		// 每月触发
		static void month(timerparm& aparm, int amonthday = 1/*1-31*/, int ahour = 0, int amin = 0, int asec = 0, int acount = 0x7fffffff)
		{
			assert(amonthday >= 1 && amonthday <= 31);
			assert(ahour >= 0 && ahour <= 23);
			assert(amin >= 0 && amin <= 59);
			assert(asec >= 0 && asec <= 59);
			aparm.m_type = timerparm::ET_WEEK;
			aparm.m_ms = month_ms(localtime::gettime(), amonthday, ahour, amin, asec);
			aparm.m_intervalms = [amonthday, ahour, amin, asec](int64_t ams)
				{
					return make_timerparm::month_ms(localtime::gettime(), ams / 1000 + 1, amonthday, ahour, amin, asec);
				};
			aparm.m_count = acount;
		}
		// 1-7
		static void week(timerparm& aparm, int aweek = 1/*1-7*/, int ahour = 0, int amin = 0, int asec = 0, int acount = 0x7fffffff)
		{
			assert(aweek >= 1 && aweek <= 7);
			assert(ahour >= 0 && ahour <= 23);
			assert(amin >= 0 && amin <= 59);
			assert(asec >= 0 && asec <= 59);
			aparm.m_type = timerparm::ET_WEEK;
			time_t lnow = localtime::gettime();
			time_t lfirst = localtime::getweekday(aweek >= 7 ? 0 : aweek, ahour, amin, asec);
			aparm.m_ms = (lfirst - lnow) * localtime::MILLISECOND;
			aparm.m_intervalms = [](int64_t)
				{
					return localtime::WEEK_MILLISECOND;
				};
			aparm.m_count = acount;
		}
		// 每日触发  ahour时amin分asec秒
		static void day(timerparm& aparm, int ahour, int amin = 0, int asec = 0, int acount = 0x7fffffff)
		{
			assert(ahour >= 0 && ahour <= 23);
			assert(amin >= 0 && amin <= 59);
			assert(asec >= 0 && asec <= 59);
			aparm.m_type = timerparm::ET_DAY;
			time_t lnow = localtime::gettime();
			time_t lfirst = localtime::getsecond2time(ahour, amin, asec);
			aparm.m_ms = (lfirst - lnow) * localtime::MILLISECOND;
			aparm.m_intervalms = [](int64_t)
				{
					return localtime::DAY_MILLISECOND;
				};
			aparm.m_count = acount;
		}
		// 每小时触发  amin分asec秒
		static void hour(timerparm& aparm, int amin, int asec, int acount = 0x7fffffff)
		{
			assert(amin >= 0 && amin <= 59);
			assert(asec >= 0 && asec <= 59);
			aparm.m_type = timerparm::ET_HOUR;
			time_t lnow = localtime::gettime();
			time_t lfirst = localtime::getsecond2time(amin, asec);
			aparm.m_ms = (lfirst - lnow) * localtime::MILLISECOND;
			aparm.m_intervalms = [](int64_t)
				{
					return localtime::HOUR_MILLISECOND;
				};
			aparm.m_count = acount;
		}
		// 每分钟触发  asec秒
		static void min(timerparm& aparm, int asec, int acount = 0x7fffffff)
		{
			assert(asec >= 0 && asec <= 59);
			aparm.m_type = timerparm::ET_MIN;
			time_t lnow = localtime::gettime();
			time_t lfirst = localtime::getsecond2time(asec);
			aparm.m_ms = (lfirst - lnow) * localtime::MILLISECOND;
			aparm.m_intervalms = [](int64_t)
				{
					return localtime::MINUTES_MILLISECOND;
				};
			aparm.m_count = acount;
		}
		// 每n秒触发
		static void make_interval(timerparm& aparm, int asec, int acount = 0x7fffffff)
		{
			assert(asec >= 0);
			aparm.m_type = timerparm::ET_INTERVAL_SEC;
			aparm.m_ms = asec * localtime::MILLISECOND;
			aparm.m_intervalms = [asec](int64_t)
				{
					return asec * localtime::MILLISECOND;
				};
			aparm.m_count = acount;
		}
	};

	class actor_base;

	class actor_timer
	{
	public:
		// 抛出一个定时器
		static int addtimer(actor_base* actor, std::shared_ptr<timerparm>& aparm);
	};
}
