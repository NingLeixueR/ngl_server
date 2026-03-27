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
// File overview: Implements logic for tools.


#include "tools/tools/tools_time.h"

#include <iostream>
#include <chrono>
#include <ctime>

namespace ngl::tools::time
{
	time_t m_offset = 0;

	bool check_month(int amonth/*1-12*/)
	{
		return amonth >= 1 && amonth <= 12;
	}

	bool check_monthday(int amonthday/*1-31*/)
	{
		return amonthday >= 1 && amonthday <= 31;
	}

	bool check_week(int aweek/*1-7*/)
	{
		return aweek >= 1 && aweek <= 7;
	}

	bool check_hour(int hour/*0-23*/)
	{
		return hour >= 0 && hour <= 23;
	}

	bool check_minute(int minute/*0-59*/)
	{
		return minute >= 0 && minute <= 59;
	}

	bool check_sec(int sec/*0-59*/)
	{
		return sec >= 0 && sec <= 59;
	}

	bool timeout(time_t abeg, int32_t atimeoutms)
	{
		return (gettimems() - abeg) > atimeoutms;
	}

	bool settime(time_t sti)
	{
		time_t lnow = gettime();
		if (lnow > sti)
		{
			return false;
		}
		// Reset first so the new offset is measured against raw current time.
		m_offset = 0;
		lnow = gettime();
		m_offset = (sti - lnow);
		return true;
	}

	time_t gettime()
	{
		time_t lnow = std::time(nullptr);
		return m_offset + lnow;
	}

	time_t getsystime()
	{
		time_t lnow = std::time(nullptr);
		return lnow;
	}

	time_t getms()
	{
		// Convenience helper for code that needs the sub-second portion only.
		return gettimems() % MILLISECOND;
	}

	time_t gettimems()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	bool issameday(time_t a1, time_t a2)
	{
		return getsecond2time(a1, 0, 0, 0) == getsecond2time(a2, 0, 0, 0);
	}

	bool issameday(time_t autc)
	{
		return issameday(autc, gettime());
	}

	void printf_time2str(time_t anow, const char* format /*= "%Y-%m-%d %H:%M:%S"*/)
	{//Year-Month-Day Hour:Minuts:Second %y-%m-%d %H:%M:%S
		char lbuff[1024];
		time2str(lbuff, 1024, anow, format);
		std::cout << lbuff;
	}

	size_t time2str(char* str, int len, time_t anow, const char* format /*= "%Y-%m-%d %H:%M:%S"*/)
	{//Year-Month-Day Hour:Minuts:Second %y-%m-%d %H:%M:%S
		time_t curr = ((anow < 0) ? gettime() : anow);
		std::tm tmTime;
#ifdef _WIN32
		localtime_s(&tmTime, &curr);
#else
		localtime_r(&curr, &tmTime);
#endif
		return strftime(str, len, format, &tmTime);
	}

	std::string time2str(time_t anow, const char* format)
	{
		char lbuff[1024] = { 0 };
		size_t lindex = time2str(lbuff, 1024, anow, format);
		if (lindex < 0)
		{
			return "";
		}
		return lbuff;
	}

	std::string time2str(const char* format)
	{
		return time2str(gettime(), format);
	}

	time_t str2time(const char* astr, const char* format/* = "%Y-%m-%d %H:%M:%S"*/)
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

	time_t getsecond2time(time_t utc, int hour, int minute, int sec/*0-59*/)
	{
		std::tm tmTime;
		gettm(utc, tmTime);
		if (check_hour(hour))
		{
			tmTime.tm_hour = hour;
		}
		if (check_minute(minute))
		{
			tmTime.tm_min = minute;
		}
		if (check_sec(sec))
		{
			tmTime.tm_sec = sec;
		}
		return mktime(&tmTime);
	}

	time_t getsecond2time(int hour, int minute, int sec/*0-59*/)
	{
		time_t lnow = gettime();
		time_t ltemp = getsecond2time(lnow, hour, minute, sec);
		// If today's scheduled time already passed, roll forward to tomorrow.
		return lnow > ltemp ? ltemp + DAY_SECOND : ltemp;
	}

	time_t getsecond2time(int minute/*0-59*/, int sec/*0-59*/)
	{
		time_t lnow = gettime();
		time_t ltemp = getsecond2time(lnow, -1, minute, sec);
		return lnow > ltemp ? ltemp + HOUR_SECOND : ltemp;
	}

	time_t getsecond2time(int sec/*0-59*/)
	{
		time_t lnow = gettime();
		time_t ltemp = getsecond2time(lnow, -1, -1, sec);
		return lnow > ltemp ? ltemp + MINUTES_SECOND : ltemp;
	}

	int getutcbyhour(time_t utc, int hour)
	{
		time_t ltemp = utc + ((int)HOUR_SECOND * hour);
		std::tm tmTime;
		gettm(ltemp, tmTime);
		tmTime.tm_min = 0;
		tmTime.tm_sec = 0;
		return (int)mktime(&tmTime);
	}

	int getutcbymin(time_t utc, int amin)
	{
		time_t ltemp = utc + ((int)MINUTES_SECOND * amin);
		std::tm ltime;
		gettm(ltemp, ltime);
		ltime.tm_sec = 0;
		return (int)mktime(&ltime);
	}

	time_t getweekday(time_t utc, int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/)
	{
		time_t lret = getsecond2time(utc, hour, minute, sec);
		std::tm tmTime;
		gettm(utc, tmTime);
		int lweek = tmTime.tm_wday == 0 ? 7 : tmTime.tm_wday;
		return lret + (aweek - lweek) * DAY_SECOND;
	}

	time_t getweekday(int aweek, int hour, int minute, int sec/*0-59*/)
	{
		return getweekday(gettime(), aweek, hour, minute, sec);
	}

	bool isleapyear(int year)
	{
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
		{
			return true;
		}
		return false;
	}

	bool mothday(int year, int month, int aday)
	{
		if (check_month(month) == false)
		{
			return false;
		}
		const int* lpday = nullptr;
		static int lday[2][12] =
		{
			{31,29,31,30,31,30,31,31,30,31,30,31},
			{31,28,31,30,31,30,31,31,30,31,30,31}
		};
		if (isleapyear(year))
		{
			lpday = lday[0];
		}
		else
		{
			lpday = lday[1];
		}

		if (aday > lpday[month - 1] || aday <= 0)
		{
			return false;
		}
		return true;
	}

	std::pair<bool, time_t> getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/)
	{
		std::tm ltime;
		gettm(utc, ltime);

		if (!check_hour(hour) || !check_minute(minute) || !check_sec(sec))
		{
			return std::make_pair(false, -1);
		}

		if (mothday(1900 + ltime.tm_year, ltime.tm_mon + 1, amday) == false)
		{
			return std::make_pair(false, -1);
		}
		ltime.tm_mday = amday;
		ltime.tm_hour = hour;
		ltime.tm_min = minute;
		ltime.tm_sec = sec;
		return std::make_pair(true, mktime(&ltime));
	}

	std::pair<bool, time_t> getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/)
	{
		return getmothday(gettime(), amday, hour, minute, sec);
	}

	time_t getspandays(time_t curr, time_t last)
	{
		time_t ltempcurr = getsecond2time(curr, 0, 0, 0);
		time_t ltemplast = getsecond2time(last, 0, 0, 0);
		return (ltempcurr - ltemplast) / DAY_SECOND;
	}


#ifdef WIN32
# define localtime_r(A,B) localtime_s(B,A)
#endif

	void gettm(time_t curr, tm& atm)
	{
		localtime_r(&curr, &atm);
	}

	void getweekday(time_t curr, int& weekday, int& hour, int& minute)
	{
		std::tm ltime;
		gettm(curr, ltime);

		weekday = ltime.tm_wday;
		hour = ltime.tm_hour;
		minute = ltime.tm_min;
	}

	int getweekday(const tm* atm)
	{
		return atm->tm_wday;
	}

	int getweekday(time_t curr)
	{
		std::tm ltime;
		gettm(curr, ltime);
		return getweekday(&ltime);
	}

	int getmoonday(const tm* atm)
	{
		return atm->tm_mday;
	}

	int getmoonday(time_t curr)
	{
		std::tm ltime;
		gettm(curr, ltime);
		return getmoonday(&ltime);
	}

	int getmoon(const tm* atm)
	{
		return atm->tm_mon + 1;
	}

	int getmoon(time_t curr)
	{
		std::tm ltime;
		gettm(curr, ltime);
		return getmoon(&ltime);
	}

	int getyear(const tm* atm)
	{
		return 1900 + atm->tm_year;
	}


	int getyear(time_t curr)
	{
		std::tm ltime;
		gettm(curr, ltime);
		return getyear(&ltime);
	}

	int gethour(const tm* atm)
	{
		return atm->tm_hour;
	}

	int gethour(time_t curr)
	{
		std::tm ltime;
		gettm(curr, ltime);
		return gethour(&ltime);
	}

	int getmin(const tm* atm)
	{
		return atm->tm_min;
	}

	int getmin(time_t curr)
	{
		std::tm ltime;
		gettm(curr, ltime);
		return getmin(&ltime);
	}

	int getsec(const tm* atm)
	{
		return atm->tm_sec;
	}

	int getsec(time_t curr)
	{
		std::tm ltime;
		gettm(curr, ltime);
		return getsec(&ltime);
	}

	int getweekday()
	{
		return getweekday(gettime());
	}

	int gethour()
	{
		return gethour(gettime());
	}
	int getmin()
	{
		return getmin(gettime());
	}

	int getsec()
	{
		return getsec(gettime());
	}

	int getyear()
	{
		return getyear(gettime());
	}

	int getmoon()
	{
		return getmoon(gettime());
	}

	int getmoonday()
	{
		return getmoonday(gettime());
	}

	bool checkutc(time_t autc1, time_t autc2)
	{
		return autc1 > autc2;
	}

	bool checkutc(time_t autc)
	{
		return checkutc(autc, gettime());
	}

	bool issameweek(time_t timestamp1, time_t timestamp2)
	{
		// Normalize both timestamps to the start of their Monday-equivalent week window.
		return (getweekday(timestamp1, 1, 0, 0, 0) == getweekday(timestamp2, 1, 0, 0, 0));
	}

	void test_isweek()
	{
		time_t ltemp1 = gettime();
		time_t ltemp2 = ltemp1 - 10 * DAY_SECOND;
		for (int i = 0; i < 20; ++i, ltemp2 += DAY_SECOND)
		{
			std::cout << getmoon(ltemp2) << "." << getmoonday(ltemp2) << ":" << (issameweek(ltemp1, ltemp2) ? "yes" : "no") << std::endl;
		}
	}
}// namespace ngl
