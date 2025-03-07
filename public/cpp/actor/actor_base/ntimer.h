#pragma once

#include "time_wheel.h"
#include "nprotocol.h"
#include "serialize.h"
#include "ndefine.h"
#include "pack.h"

#include <memory>

namespace ngl
{
	struct timerparm
	{
		enum E_ACTOR_TIMER
		{
			ET_NULL,
			ET_MONTH,		// ÿ�´���
			ET_WEEK,		// ÿ�ܴ���
			ET_DAY,			// ÿ�մ���		ahourʱamin��asec��
			ET_HOUR,		// ÿСʱ����	amin��asec��
			ET_MIN,			// ÿ���Ӵ���	asec��
			ET_INTERVAL_SEC,// ���n�봥��
		};
		int								m_type = 0;
		int								m_timerid = 0;
		int64_t							m_ms = 0;					// ����ڵ�ǰʱ�� ms
		int								m_count = 1;				// ��������
		std::function<int32_t(int64_t)> m_intervalms = nullptr;		// �������
		std::shared_ptr<void>			m_parm = nullptr;			// �Զ������
		int64_t							m_triggerms = 0;			// ����ʱ�ĺ���

		template <typename T>
		void set_parm(std::shared_ptr<T>& aparm)
		{
			m_parm = std::static_pointer_cast<void>(aparm);
		}

		def_portocol(timerparm, m_type, m_timerid, m_ms, m_count, m_triggerms)
	};

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

		//ET_MONTH,		// ÿ�´���
		static bool month(timerparm& aparm, int amonthday/*1-31*/, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (localtime::check_monthday(amonthday) && localtime::check_hour(ahour) && localtime::check_minute(amin) && localtime::check_sec(asec))
			{
				aparm.m_type = timerparm::ET_WEEK;
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

		// ÿ�ܴ��� 1-7
		static bool week(timerparm& aparm, int aweek/*1-7*/, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (localtime::check_week(aweek) && localtime::check_hour(ahour) && localtime::check_minute(amin) && localtime::check_sec(asec))
			{
				aparm.m_type = timerparm::ET_WEEK;
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

		// ÿ�մ���  ahourʱamin��asec��
		static bool day(timerparm& aparm, int ahour, int amin, int asec, int acount = 0x7fffffff)
		{
			if (localtime::check_hour(ahour) && localtime::check_minute(amin) && localtime::check_sec(asec))
			{
				aparm.m_type = timerparm::ET_DAY;
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

		// ÿСʱ����  amin��asec��
		static bool hour(timerparm& aparm, int amin, int asec, int acount = 0x7fffffff)
		{
			if (localtime::check_minute(amin) && localtime::check_sec(asec))
			{
				aparm.m_type = timerparm::ET_HOUR;
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

		// ÿ���Ӵ���  asec��
		static bool min(timerparm& aparm, int asec, int acount = 0x7fffffff)
		{
			if (localtime::check_sec(asec))
			{
				aparm.m_type = timerparm::ET_MIN;
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

		// ÿn�봥��
		static bool make_interval(timerparm& aparm, int asec, int acount = 0x7fffffff)
		{
			if (asec >= 0)
			{
				aparm.m_type = timerparm::ET_INTERVAL_SEC;
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
		// �׳�һ����ʱ��
		static int addtimer(actor_base* actor, std::shared_ptr<timerparm>& aparm);
	};
}//namespace ngl
