#include "localtime.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>

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
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::hours(8);
		const boost::posix_time::time_duration td = now.time_of_day();
		int64_t hh = td.hours();
		int64_t mm = td.minutes();
		int64_t ss = td.seconds();
		return now.time_of_day().total_milliseconds() - ((hh * 3600 + mm * 60 + ss) * 1000);
	}

	time_t localtime::gettimems()
	{
		return gettime() * 1000 + getms();
	}

	void localtime::printf_time2str(time_t anow, const char* format)
	{//Year-Month-Day Hour:Minuts:Second %y-%m-%d %H:%M:%S
		char lbuff[1024];
		time_t curr = ((anow == 0) ? gettime() : anow);

		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&curr, &tmTime);
		strftime(lbuff, 1024, format, ltime);
		std::cout << lbuff;
	}

	size_t localtime::time2str(char* str, int len, time_t anow, const char* format)
	{//Year-Month-Day Hour:Minuts:Second %y-%m-%d %H:%M:%S
		time_t curr = ((anow == 0) ? gettime() : anow);

		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&curr, &tmTime);
		return strftime(str, len, format, ltime);
	}

	size_t localtime::time2msstr(char* str, int len, time_t anow, const char* format)
	{
		size_t lindex = time2str(str, len, anow, format);
		if (lindex < 0)
			return lindex;
		if (lindex + 5 > len)
			return -1;
		str[lindex] = '.';

		time_t ltemp = anow % 1000;
		str[lindex + 1] = '0' + ltemp / 100;
		ltemp %= 100;
		str[lindex + 2] = '0' + ltemp / 10;
		str[lindex + 3] = '0' + ltemp % 10;
		str[lindex + 4] = '\0';
		return lindex + 5;
	}

	std::string localtime::time2msstr(time_t anow, const char* format)
	{
		char lbuff[1024] = { 0 };
		size_t lindex = time2str(lbuff, 1024, anow, format);
		if (lindex < 0)
			return "";
		if (lindex + 5 > 1024)
			return "";
		lbuff[lindex] = '.';

		time_t ltemp = anow % 1000;
		lbuff[lindex + 1] = '0' + ltemp / 100;
		ltemp %= 100;
		lbuff[lindex + 2] = '0' + ltemp / 10;
		lbuff[lindex + 3] = '0' + ltemp % 10;
		lbuff[lindex + 4] = '\0';
		return lbuff;
	}

	std::string localtime::time2msstr(const char* format)
	{
		return time2msstr(gettime(), format);
	}

	/*const char* localtime::time2str(int anow, const char* format)
	{
		static char lbuff[1024] = { 0 };
		time2str(lbuff, 1024, anow, format);
		return lbuff;
	}*/

	time_t localtime::st2time(const char* str)
	{
		tm date;
		char strTime[32] = { 0 };
		int ret = sscanf(str, 
			"%d/%d/%d %d:%d:%d", 
			&date.tm_year, 
			&date.tm_mon, 
			&date.tm_mday,
			&date.tm_hour,
			&date.tm_min,
			&date.tm_sec
			);
		if (ret == 6)
		{
			date.tm_mon -= 1;
			date.tm_year -= 1900;
			date.tm_isdst = -1;
			return mktime(&date);
		}
		return -1;
	}

	//获取utc时刻那天的 小时分钟的utc
	time_t localtime::getsecond2time(time_t utc, int hour, int minute, int sec/*0-59*/)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&utc, &tmTime);
		//struct tm* ltime = localtime(&curr);
		ltime->tm_hour = hour;
		ltime->tm_min = minute;
		ltime->tm_sec = sec;
		return mktime(ltime);
	}

	time_t localtime::getsecond2time(int hour, int minute, int sec/*0-59*/)
	{
		time_t lnow = gettime();
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&lnow, &tmTime);
		//struct tm* ltime = localtime(&curr);

		ltime->tm_hour = hour;
		ltime->tm_min = minute;
		ltime->tm_sec = sec;
		time_t lret = mktime(ltime);
		if (lret > lnow)
		{
			return lret;
		}
		else
		{
			return lret + (24 * 60 * 60);
		}
	}

	time_t localtime::getsecond2time(int minute/*0-59*/, int sec/*0-59*/)
	{
		time_t lnow = gettime();
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&lnow, &tmTime);
		//struct tm* ltime = localtime(&curr);

		ltime->tm_min = minute;
		ltime->tm_sec = sec;
		time_t lret = mktime(ltime);
		if (lret > lnow)
		{
			return lret;
		}
		else
		{
			return lret + (1 * 60 * 60);
		}
	}

	time_t localtime::getsecond2time(int sec/*0-59*/)
	{
		time_t lnow = gettime();
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&lnow, &tmTime);

		ltime->tm_sec = sec;
		time_t lret = mktime(ltime);
		if (lret > lnow)
		{
			return lret;
		}
		else
		{
			return lret + (1 * 60);
		}
	}

	int localtime::getutcbyhour(time_t utc, int hour)
	{
		time_t ltemp = utc + ((int)HOUR_SECOND * hour);
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&ltemp, &tmTime);
		ltime->tm_min = 0;
		ltime->tm_sec = 0;
		return mktime(ltime);
	}

	int localtime::getutcbymin(time_t utc, int min)
	{
		time_t ltemp = utc + ((int)MINUTES_SECOND * min);
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&ltemp, &tmTime);
		ltime->tm_sec = 0;
		return mktime(ltime);
	}

	time_t localtime::getweekday(time_t utc, int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&utc, &tmTime);

		ltime->tm_hour = hour;
		ltime->tm_min = minute;
		ltime->tm_sec = sec;
		//return mktime(ltime);
		time_t lret = mktime(ltime);
		int lweek = aweek - ltime->tm_wday;
		if (lweek == 0)
		{
			return lret;
		}
		else
		{
			return lret + (lweek * 24 * 60 * 60);
		}
	}

	time_t localtime::getweekday(int aweek, int hour, int minute, int sec/*0-59*/)
	{
		return getweekday(gettime(), aweek, hour, minute, sec);
	}

	time_t localtime::getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&utc, &tmTime);

		ltime->tm_mday = amday;
		ltime->tm_hour = hour;
		ltime->tm_min = minute;
		ltime->tm_sec = sec;
		time_t lret = mktime(ltime);
		if (utc < lret)
		{
			return lret;
		}
		else
		{
			if (ltime->tm_mon == 11)//---0-11月 年底
			{
				++ltime->tm_year;
				ltime->tm_mon = 0;
				ltime->tm_mday = amday;
				ltime->tm_hour = hour;
				ltime->tm_min = minute;
				ltime->tm_sec = sec;
				return mktime(ltime);
			}
			++ltime->tm_mon;
			return mktime(ltime);
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
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);
		//struct tm* ltime = localtime((time_t*)&curr);
		ltime->tm_hour = 0;
		ltime->tm_min = 0;
		ltime->tm_sec = 0;
		time_t newCurr = mktime(ltime);

		ltime = boost::date_time::c_time::localtime((time_t*)&last, &tmTime);
		//ltime = localtime((time_t*)&last);
		ltime->tm_hour = 0;
		ltime->tm_min = 0;
		ltime->tm_sec = 0;
		time_t newLast = mktime(ltime);

		return (newCurr - newLast) / (24 * 60 * 60);
	}

	time_t localtime::getnextweekdaytime(time_t curr, time_t weekday)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);
		//struct tm* ltime = localtime((time_t*)&curr);
		int currWeekday = ltime->tm_wday;

		time_t weekdayInterval = weekday - currWeekday;
		if (weekdayInterval <= 0)
		{
			weekdayInterval += 7;
		}

		return curr + weekdayInterval * 24 * 60 * 60 - ltime->tm_hour * 60 * 60 - ltime->tm_min * 60 - ltime->tm_sec;
	}

	time_t localtime::getnextweekdaytime(time_t curr, int weekday, int hour, int minute)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);
		//struct tm* ltime = localtime((time_t*)&curr);
		int currWeekday = ltime->tm_wday;

		int weekdayInterval = weekday - currWeekday;
		if (weekdayInterval < 0)
		{
			weekdayInterval += 7;
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
				weekdayInterval += 7;
				return getsecond2time(curr + 7 * 24 * 60 * 60, hour, minute);
			}
		}
		return getsecond2time(curr + weekdayInterval * 24 * 60 * 60, hour, minute);
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
		return (getnextweekdaytime(curr, weekday) - getnextweekdaytime(last, weekday)) / (24 * 60 * 60 * 7);
	}

	/**
	获取指定小时，分钟时间和目前时间的差值，返回负数则表示指定时间已过 */
	time_t localtime::getdesttimevalue(int hour, int minute, int sec)
	{
		time_t curr = gettime();  // 使用服务器的当前时间
		time_t dest = curr;

		struct tm tmTime;
		struct tm* destTime = boost::date_time::c_time::localtime(&dest, &tmTime);
		//struct tm* destTime = localtime(&dest);
		destTime->tm_hour = hour;
		destTime->tm_min = minute;
		destTime->tm_sec = sec; // 精确秒数
		dest = mktime(destTime);
		return dest - curr;
	}

	/*
	获取utc那天的第x天的0点的utc时间，utc当天算第0天，utc明天算第1天
	*/
	time_t localtime::getutcbyday(time_t utc, int dayNum)
	{
		time_t utcDayUTC = getsecond2time(utc, 0, 0);
		return utcDayUTC + dayNum * 86400;

	}

	/**
	获取指定年，月，日，小时，分钟时间和目前时间的差值，返回负数则表示指定时间已过 */
	time_t localtime::getdestdayvalue(time_t curr, int year, int month, int day, int hour, int minute)
	{
		time_t dest = curr;
		struct tm tmTime;
		struct tm* destTime = boost::date_time::c_time::localtime(&dest, &tmTime);
		destTime->tm_hour = hour;
		destTime->tm_min = minute;
		destTime->tm_sec = 0; // 精确秒数
		destTime->tm_year = year - 1900;// struct tm 的年份是从1900开始的
		destTime->tm_mon = month - 1;// struct tm 的月份是从0到11的
		destTime->tm_mday = day;
		dest = mktime(destTime);
		return dest - curr;
	}

	void localtime::gettm(time_t curr, tm& atm)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);
		atm = *ltime;
	}

	void localtime::getweekday(
		time_t curr,
		int& weekday,
		int& hour, int& minute)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);
		weekday = ltime->tm_wday;
		hour = ltime->tm_hour;
		minute = ltime->tm_min;
	}

	int localtime::getweekday(const tm* atm)
	{
		return atm->tm_wday;
	}

	int localtime::getweekday(time_t curr)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);

		return getweekday(ltime);
	}

	int localtime::getmoonday(const tm* atm)
	{
		return atm->tm_mday;
	}

	int localtime::getmoonday(time_t curr)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);

		return getmoonday(ltime);
	}

	int localtime::getmoon(const tm* atm)
	{
		return atm->tm_mon + 1;
	}

	int localtime::getmoon(time_t curr)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime(&curr, &tmTime);

		return getweekday(ltime);
	}

	int localtime::getyear(const tm* atm)
	{
		return 1900 + atm->tm_year;
	}


	int localtime::getyear(time_t curr)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);

		return getyear(ltime);
	}

	int localtime::gethour(const tm* atm)
	{
		return atm->tm_hour;
	}

	int localtime::gethour(time_t curr)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);

		return gethour(ltime);
	}

	int localtime::getmin(const tm* atm)
	{
		return atm->tm_min;
	}

	int localtime::getmin(time_t curr)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);

		return getmin(ltime);
	}

	int localtime::getsec(const tm* atm)
	{
		return atm->tm_sec;
	}

	int localtime::getsec(time_t curr)
	{
		struct tm tmTime;
		struct tm* ltime = boost::date_time::c_time::localtime((time_t*)&curr, &tmTime);

		return getsec(ltime);
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
}// namespace ngl