#include "localtime.h"

#include <iostream>
#include <chrono>
#include <ctime>

namespace ngl
{
	time_t localtime::m_offset = 0;

	/** 设置时间 */
	bool localtime::settime(time_t sti)
	{
		time_t lnow = gettime();
		if (lnow > sti)
		{
			return false;
		}
		m_offset += (sti - lnow);
		return true;
	}

	/** 获取时间*/
	time_t localtime::gettime()
	{
		time_t lnow = time(nullptr);
		return m_offset + lnow;
	}

	time_t localtime::getms()
	{
		return gettimems() % 1000;
	}

	time_t localtime::gettimems()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	void localtime::printf_time2str(time_t anow, const char* format)
	{//Year-Month-Day Hour:Minuts:Second %y-%m-%d %H:%M:%S
		char lbuff[1024];
		time2str(lbuff, 1024, anow, format);
		std::cout << lbuff;
	}

	size_t localtime::time2str(char* str, int len, time_t anow, const char* format)
	{//Year-Month-Day Hour:Minuts:Second %y-%m-%d %H:%M:%S
		time_t curr = ((anow <= 0) ? gettime() : anow);
		std::tm tmTime = *std::localtime(&curr);
		return strftime(str, len, format, &tmTime);
	}

	std::string localtime::time2str(time_t anow, const char* format)
	{
		char lbuff[1024] = { 0 };
		size_t lindex = time2str(lbuff, 1024, anow, format);
		if (lindex < 0)
			return "";
		return lbuff;
	}

	std::string localtime::time2str(const char* format)
	{
		return time2str(gettime(), format);
	}

	time_t localtime::str2time(const char* astr, const char* format/* = "%Y-%m-%d %H:%M:%S"*/)
	{//%Y-%m-%d %H:%M:%S
		std::tm tm_struct = {};
		std::istringstream ss(astr);
		ss >> std::get_time(&tm_struct, format);

		if (ss.fail())
		{
			return -1;
		}
		return std::mktime(&tm_struct);
	}

	//获取utc时刻那天的 小时分钟的utc
	time_t localtime::getsecond2time(time_t utc, int hour, int minute, int sec/*0-59*/)
	{
		std::tm tmTime = *std::localtime(&utc);
		//struct tm* ltime = localtime(&curr);
		tmTime.tm_hour = hour;
		tmTime.tm_min = minute;
		tmTime.tm_sec = sec;
		return mktime(&tmTime);
	}

	time_t localtime::getsecond2time(int hour, int minute, int sec/*0-59*/)
	{
		time_t lnow = gettime();
		std::tm tmTime = *std::localtime(&lnow);

		tmTime.tm_hour = hour;
		tmTime.tm_min = minute;
		tmTime.tm_sec = sec;
		time_t lret = mktime(&tmTime);
		if (lret > lnow)
		{
			return lret;
		}
		else
		{
			return lret + DAY_SECOND;
		}
	}

	time_t localtime::getsecond2time(int minute/*0-59*/, int sec/*0-59*/)
	{
		time_t lnow = gettime();
		std::tm tmTime = *std::localtime(&lnow);
		tmTime.tm_min = minute;
		tmTime.tm_sec = sec;
		time_t lret = mktime(&tmTime);
		if (lret > lnow)
		{
			return lret;
		}
		else
		{
			return lret + HOUR_SECOND;
		}
	}

	time_t localtime::getsecond2time(int sec/*0-59*/)
	{
		time_t lnow = gettime();
		std::tm tmTime = *std::localtime(&lnow);

		tmTime.tm_sec = sec;
		time_t lret = mktime(&tmTime);
		if (lret > lnow)
		{
			return lret;
		}
		else
		{
			return lret + MINUTES_SECOND;
		}
	}

	int localtime::getutcbyhour(time_t utc, int hour)
	{
		time_t ltemp = utc + ((int)HOUR_SECOND * hour);
		std::tm tmTime = *std::localtime(&ltemp);
		tmTime.tm_min = 0;
		tmTime.tm_sec = 0;
		return mktime(&tmTime);
	}

	int localtime::getutcbymin(time_t utc, int min)
	{
		time_t ltemp = utc + ((int)MINUTES_SECOND * min);
		std::tm ltime = *std::localtime(&ltemp);
		ltime.tm_sec = 0;
		return mktime(&ltime);
	}

	time_t localtime::getweekday(time_t utc, int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/)
	{
		std::tm ltime = *std::localtime(&utc);

		ltime.tm_hour = hour;
		ltime.tm_min = minute;
		ltime.tm_sec = sec;
		time_t lret = mktime(&ltime);
		int lweek = aweek - ltime.tm_wday;
		if (lweek == 0)
		{
			return lret;
		}
		else
		{
			return lret + (lweek * DAY_SECOND);
		}
	}

	time_t localtime::getweekday(int aweek, int hour, int minute, int sec/*0-59*/)
	{
		return getweekday(gettime(), aweek, hour, minute, sec);
	}

	time_t localtime::getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/)
	{
		std::tm ltime = *std::localtime(&utc);

		ltime.tm_mday = amday;
		ltime.tm_hour = hour;
		ltime.tm_min = minute;
		ltime.tm_sec = sec;
		time_t lret = mktime(&ltime);
		if (utc < lret)
		{
			return lret;
		}
		else
		{
			if (ltime.tm_mon == 11)//---0-11月 年底
			{
				++ltime.tm_year;
				ltime.tm_mon = 0;
				ltime.tm_mday = amday;
				ltime.tm_hour = hour;
				ltime.tm_min = minute;
				ltime.tm_sec = sec;
				return mktime(&ltime);
			}
			++ltime.tm_mon;
			return mktime(&ltime);
		}
	}

	time_t localtime::getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/)
	{
		time_t lnow = gettime();
		return getmothday(lnow, amday, hour, minute, sec);
	}

	//获取两个时间之间相差的天数
	time_t localtime::getspandays(time_t curr, time_t last)
	{
		std::tm ltime = *std::localtime(&curr);
		ltime.tm_hour = 0;
		ltime.tm_min = 0;
		ltime.tm_sec = 0;
		time_t newCurr = mktime(&ltime);

		ltime = *std::localtime(&last);
		ltime.tm_hour = 0;
		ltime.tm_min = 0;
		ltime.tm_sec = 0;
		time_t newLast = mktime(&ltime);

		return (newCurr - newLast) / DAY_SECOND;
	}

	time_t localtime::getnextweekdaytime(time_t curr, time_t weekday)
	{
		std::tm ltime = *std::localtime(&curr);
		int currWeekday = ltime.tm_wday;

		time_t weekdayInterval = weekday - currWeekday;
		if (weekdayInterval <= 0)
		{
			weekdayInterval += WEEK_DAY;
		}

		return curr + weekdayInterval * DAY_SECOND - ltime.tm_hour * HOUR_SECOND - ltime.tm_min * MINUTES_SECOND - ltime.tm_sec;
	}

	time_t localtime::getnextweekdaytime(time_t curr, int weekday, int hour, int minute)
	{
		std::tm ltime = *std::localtime(&curr);
		int currWeekday = ltime.tm_wday;

		int weekdayInterval = weekday - currWeekday;
		if (weekdayInterval < 0)
		{
			weekdayInterval += WEEK_DAY;
		}
		else if (weekdayInterval == 0)
		{
			time_t ltemp = getsecond2time(curr, hour, minute);
			if (ltemp > curr)
			{
				return ltemp;
			}
			else
			{
				weekdayInterval += WEEK_DAY;
				return getsecond2time(curr + WEEK_SECOND, hour, minute);
			}
		}
		return getsecond2time(curr + weekdayInterval * DAY_SECOND, hour, minute);
	}

	bool localtime::isspanweekday(time_t curr, time_t last, int weekday)
	{
		return last >= getnextweekdaytime(curr, weekday);
	}

	bool localtime::isspanweekday(time_t curr, time_t last, int weekday, int hour, int minute)
	{
		return last >= getnextweekdaytime(curr, weekday, hour, minute);
	}

	//获得两个时间之间相差多少周
	time_t localtime::getspanweeks(time_t curr, time_t last, int weekday)
	{
		return (getnextweekdaytime(curr, weekday) - getnextweekdaytime(last, weekday)) / WEEK_SECOND;
	}

	/**
	获取指定小时，分钟时间和目前时间的差值，返回负数则表示指定时间已过 */
	time_t localtime::getdesttimevalue(int hour, int minute, int sec)
	{
		time_t curr = gettime();  // 使用服务器的当前时间
		time_t dest = curr;

		std::tm ltime = *std::localtime(&dest);
		ltime.tm_hour = hour;
		ltime.tm_min = minute;
		ltime.tm_sec = sec; // 精确秒数
		dest = mktime(&ltime);
		return dest - curr;
	}

	/*
	获取utc那天的第x天的0点的utc时间，utc当天算第0天，utc明天算第1天
	*/
	time_t localtime::getutcbyday(time_t utc, int dayNum)
	{
		time_t utcDayUTC = getsecond2time(utc, 0, 0);
		return utcDayUTC + dayNum * DAY_SECOND;

	}

	/**
	获取指定年，月，日，小时，分钟时间和目前时间的差值，返回负数则表示指定时间已过 */
	time_t localtime::getdestdayvalue(time_t curr, int year, int month, int day, int hour, int minute)
	{
		time_t dest = curr;
		std::tm ltime = *std::localtime(&dest);
		ltime.tm_hour = hour;
		ltime.tm_min = minute;
		ltime.tm_sec = 0; // 精确秒数
		ltime.tm_year = year - 1900;// struct tm 的年份是从1900开始的
		ltime.tm_mon = month - 1;// struct tm 的月份是从0到11的
		ltime.tm_mday = day;
		dest = mktime(&ltime);
		return dest - curr;
	}

	void localtime::gettm(time_t curr, tm& atm)
	{
		atm = *std::localtime(&curr);
	}

	void localtime::getweekday(time_t curr, int& weekday, int& hour, int& minute)
	{
		std::tm ltime = *std::localtime(&curr);
		weekday = ltime.tm_wday;
		hour = ltime.tm_hour;
		minute = ltime.tm_min;
	}

	int localtime::getweekday(const tm* atm)
	{
		return atm->tm_wday;
	}

	int localtime::getweekday(time_t curr)
	{
		std::tm ltime = *std::localtime(&curr);
		return getweekday(&ltime);
	}

	int localtime::getmoonday(const tm* atm)
	{
		return atm->tm_mday;
	}

	int localtime::getmoonday(time_t curr)
	{
		std::tm ltime = *std::localtime(&curr);
		return getmoonday(&ltime);
	}

	int localtime::getmoon(const tm* atm)
	{
		return atm->tm_mon + 1;
	}

	int localtime::getmoon(time_t curr)
	{
		std::tm ltime = *std::localtime(&curr);
		return getweekday(&ltime);
	}

	int localtime::getyear(const tm* atm)
	{
		return 1900 + atm->tm_year;
	}


	int localtime::getyear(time_t curr)
	{
		std::tm ltime = *std::localtime(&curr);
		return getyear(&ltime);
	}

	int localtime::gethour(const tm* atm)
	{
		return atm->tm_hour;
	}

	int localtime::gethour(time_t curr)
	{
		std::tm ltime = *std::localtime(&curr);
		return gethour(&ltime);
	}

	int localtime::getmin(const tm* atm)
	{
		return atm->tm_min;
	}

	int localtime::getmin(time_t curr)
	{
		std::tm ltime = *std::localtime(&curr);
		return getmin(&ltime);
	}

	int localtime::getsec(const tm* atm)
	{
		return atm->tm_sec;
	}

	int localtime::getsec(time_t curr)
	{
		std::tm ltime = *std::localtime(&curr);
		return getsec(&ltime);
	}

	int localtime::getweekday()
	{
		return getweekday(gettime());
	}

	int localtime::gethour()
	{
		return gethour(gettime());
	}
	int localtime::getmin()
	{
		return getmin(gettime());
	}

	int localtime::getsec()
	{
		return getsec(gettime());
	}

	int localtime::getyear()
	{
		return getyear(gettime());
	}

	int localtime::getmoon()
	{
		return getmoon(gettime());
	}

	int localtime::getmoonday()
	{
		return getmoonday(gettime());
	}

	bool localtime::issameweek(time_t timestamp1, time_t timestamp2)
	{
		timestamp1 = getsecond2time(timestamp1, 0, 0);
		timestamp2 = getsecond2time(timestamp2, 0, 0);
		std::tm* timeinfo1 = std::localtime(&timestamp1);
		std::tm* timeinfo2 = std::localtime(&timestamp2);

		// 获取星期几
		int dayOfWeek1 = timeinfo1->tm_wday;
		int dayOfWeek2 = timeinfo2->tm_wday;

		// 获取当前日期是本周的第几天
		int daysToMonday1 = (dayOfWeek1 + 6) % 7;
		int daysToMonday2 = (dayOfWeek2 + 6) % 7;

		// 获取当前日期的起始时间（凌晨00:00:00）
		std::time_t startOfWeek1 = timestamp1 - daysToMonday1 * 24 * 60 * 60;
		std::time_t startOfWeek2 = timestamp2 - daysToMonday2 * 24 * 60 * 60;

		// 判断两个时间是否在同一周
		return (startOfWeek1 == startOfWeek2);
	}
}// namespace ngl