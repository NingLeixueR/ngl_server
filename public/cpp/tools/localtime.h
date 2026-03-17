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
// File overview: Declares interfaces for tools.

#pragma once

#include <cstdio>
#include <string>
#include <ctime>

namespace ngl
{
	class localtime
	{
	public:
		enum
		{
			MILLISECOND			= 1000,
			MINUTES_SECOND		= 1 * 60,							// Seconds per minute.
			MINUTES_MILLISECOND	= MINUTES_SECOND * MILLISECOND,		// Milliseconds per minute.
			
			HOUR_MINUTES		= 1 * 60,							// Minutes per hour.
			HOUR_SECOND			= MINUTES_SECOND * HOUR_MINUTES,	// Seconds per hour.
			HOUR_MILLISECOND	= HOUR_SECOND * MILLISECOND,		// Milliseconds per hour.

			DAY_HOUR			= 24,								// Hours per day.
			DAY_MINUTES			= DAY_HOUR * HOUR_MINUTES,			// Minutes per day.
			DAY_SECOND			= DAY_HOUR * HOUR_SECOND,			// Seconds per day.
			DAY_MILLISECOND		= DAY_SECOND * MILLISECOND,			// Milliseconds per day.

			WEEK_DAY			= 7,								// Days per week.
			WEEK_HOUR			= WEEK_DAY * DAY_HOUR,				// Hours per week.
			WEEK_MINUTES		= WEEK_DAY * DAY_MINUTES,			// Minutes per week.
			WEEK_SECOND			= WEEK_DAY * DAY_SECOND,			// Seconds per week.
			WEEK_MILLISECOND	= WEEK_SECOND * MILLISECOND,		// Milliseconds per week.
		};
		// Validation helpers for calendar/time-of-day components.
		static bool check_month(int amonth/*1-12*/);

		// # In
		static bool check_monthday(int amonthday/*1-31*/);

		// # In
		static bool check_week(int aweek/*1-7*/);

		// Translated comment.
		static bool check_hour(int hour/*0-23*/);

		// Translated comment.
		static bool check_minute(int minute/*0-59*/);

		// Translated comment.
		static bool check_sec(int sec/*0-59*/);

		// Return whether the elapsed wall-clock time exceeds atimeoutms.
		static bool timeout(time_t abeg, int32_t atimeoutms);

		// Apply a positive offset so localtime::gettime() can follow a server-synchronized clock.
		static bool settime(time_t sti);

		// Return logical server time, raw system time, milliseconds, and millisecond remainder.
		static time_t gettime();

		// # Gettime
		static time_t getsystime();

		// # Get and
		static time_t getms();

		// # Get
		static time_t gettimems();

		// Date-comparison helpers.
		static bool issameday(time_t a1, time_t a2);

		// # Currenttime parametersutctimewhether
		static bool issameday(time_t autc);

		// Format time_t values into printable strings.
		static void printf_time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");

		// # Utcconvert string
		static size_t time2str(char* str, int len, time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
		static std::string time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
		static std::string time2str(const char* format = "%Y-%m-%d %H:%M:%S");

		// Parse formatted local time strings back into time_t.
		static time_t str2time(const char* astr, const char* format = "%Y-%m-%d %H:%M:%S");

		// Build the timestamp for a specific time-of-day on a given day or the next matching day.
		static time_t getsecond2time(time_t utc, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # Get specified utc(iftime getunder specified utc)
		static time_t getsecond2time(int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # Getspecified utc(ifcurrent getunder )
		static time_t getsecond2time(int minute/*0-59*/, int sec/*0-59*/);

		// # Getspecified utc(ifcurrent getunder )
		static time_t getsecond2time(int sec/*0-59*/);

		// Build timestamps for specific weekday/month-day schedules.
		static time_t getweekday(time_t utc, int aweek/*1-7*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # Get specified utc(iftime getunder specified utc)
		static time_t getweekday(int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// Calendar helpers.
		static bool isleapyear(int year);

		// # Whether
		static bool mothday(int year, int month, int aday);

		// # Getspecifiedutc
		static std::pair<bool,time_t> getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # Get specified utc
		static std::pair<bool, time_t> getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// Return whole-day difference between two timestamps after truncating to midnight.
		static time_t getspandays(time_t curr, time_t last);

		// Translated comment.
		static int getmoonday(const tm* atm);
		static int getmoonday(time_t curr);
		static int getmoonday();

		// Translated comment.
		static int getmoon(const tm* atm);
		static int getmoon(time_t curr);
		static int getmoon();

		// Translated comment.
		static int getyear(const tm* atm);
		static int getyear(time_t curr);
		static int getyear();

		// Translated comment.
		static int getweekday(const tm* atm);
		static int getweekday(time_t curr);
		static int getweekday();

		// Translated comment.
		static int gethour(const tm* atm);
		static int gethour(time_t curr);
		static int gethour();

		// Translated comment.
		static int getmin(const tm* atm);
		static int getmin(time_t curr);
		static int getmin();

		// Translated comment.
		static int getsec(const tm* atm);
		static int getsec(time_t curr);
		static int getsec();

		// # Get:
		static void getweekday(time_t curr, int& weekday, int& hour, int& minute);

		// # Gettm
		static void gettm(time_t curr, tm& atm);

		// # Getspecifiedutcafter
		static int getutcbyhour(time_t utc, int hour);
		static int getutcbymin(time_t utc, int amin);

		static bool checkutc(time_t autc1, time_t autc2);
		static bool checkutc(time_t autc);

		static bool issameweek(time_t timestamp1, time_t timestamp2);
	private:
		static time_t m_offset;		// Logical time offset applied on top of system time.
	};

	void test_isweek();
}// namespace ngl
