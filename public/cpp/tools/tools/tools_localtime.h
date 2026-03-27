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

namespace ngl::tools::localtime
{
	enum
	{
		MILLISECOND = 1000,
		MINUTES_SECOND = 1 * 60,							// Seconds per minute.
		MINUTES_MILLISECOND = MINUTES_SECOND * MILLISECOND,		// Milliseconds per minute.

		HOUR_MINUTES = 1 * 60,							// Minutes per hour.
		HOUR_SECOND = MINUTES_SECOND * HOUR_MINUTES,	// Seconds per hour.
		HOUR_MILLISECOND = HOUR_SECOND * MILLISECOND,		// Milliseconds per hour.

		DAY_HOUR = 24,								// Hours per day.
		DAY_MINUTES = DAY_HOUR * HOUR_MINUTES,			// Minutes per day.
		DAY_SECOND = DAY_HOUR * HOUR_SECOND,			// Seconds per day.
		DAY_MILLISECOND = DAY_SECOND * MILLISECOND,			// Milliseconds per day.

		WEEK_DAY = 7,								// Days per week.
		WEEK_HOUR = WEEK_DAY * DAY_HOUR,				// Hours per week.
		WEEK_MINUTES = WEEK_DAY * DAY_MINUTES,			// Minutes per week.
		WEEK_SECOND = WEEK_DAY * DAY_SECOND,			// Seconds per week.
		WEEK_MILLISECOND = WEEK_SECOND * MILLISECOND,		// Milliseconds per week.
	};
	// Validation helpers for calendar/time-of-day components.
	static bool check_month(int amonth/*1-12*/);

	// Validate a day-of-month value.
	static bool check_monthday(int amonthday/*1-31*/);

	// Validate a weekday value using the project's 1-7 convention.
	static bool check_week(int aweek/*1-7*/);

	// Validate an hour-of-day value.
	static bool check_hour(int hour/*0-23*/);

	// Validate a minute value.
	static bool check_minute(int minute/*0-59*/);

	// Validate a second value.
	static bool check_sec(int sec/*0-59*/);

	// Return whether the elapsed wall-clock time exceeds atimeoutms.
	static bool timeout(time_t abeg, int32_t atimeoutms);

	// Apply a positive offset so tools::localtime::gettime() can follow a server-synchronized clock.
	static bool settime(time_t sti);

	// Return logical server time, raw system time, milliseconds, and millisecond remainder.
	static time_t gettime();

	// Return raw system time in seconds without the logical offset.
	static time_t getsystime();

	// Return raw system time in milliseconds.
	static time_t getms();

	// Return logical server time in milliseconds.
	static time_t gettimems();

	// Date-comparison helpers.
	static bool issameday(time_t a1, time_t a2);

	// Compare the current logical day against the supplied timestamp.
	static bool issameday(time_t autc);

	// Format time_t values into printable strings.
	static void printf_time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");

	// Write a formatted time string into a caller-provided buffer.
	static size_t time2str(char* str, int len, time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
	static std::string time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
	static std::string time2str(const char* format = "%Y-%m-%d %H:%M:%S");

	// Parse formatted local time strings back into time_t.
	static time_t str2time(const char* astr, const char* format = "%Y-%m-%d %H:%M:%S");

	// Build the timestamp for a specific time-of-day on a given day or the next matching day.
	static time_t getsecond2time(time_t utc, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Build the next matching timestamp from the current logical day.
	static time_t getsecond2time(int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Build the next matching timestamp from the current logical hour.
	static time_t getsecond2time(int minute/*0-59*/, int sec/*0-59*/);

	// Build the next matching timestamp from the current logical minute.
	static time_t getsecond2time(int sec/*0-59*/);

	// Build timestamps for specific weekday/month-day schedules.
	static time_t getweekday(time_t utc, int aweek/*1-7*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Build the next timestamp for the requested weekday and time-of-day.
	static time_t getweekday(int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Calendar helpers.
	static bool isleapyear(int year);

	// Return whether the supplied year/month/day triple is a valid date.
	static bool mothday(int year, int month, int aday);

	// Build the next timestamp for the requested day-of-month and time-of-day.
	static std::pair<bool, time_t> getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Convenience overload that uses the current logical time as the base.
	static std::pair<bool, time_t> getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Return whole-day difference between two timestamps after truncating to midnight.
	static time_t getspandays(time_t curr, time_t last);

	// Extract the day-of-month component.
	static int getmoonday(const tm* atm);
	static int getmoonday(time_t curr);
	static int getmoonday();

	// Extract the month component.
	static int getmoon(const tm* atm);
	static int getmoon(time_t curr);
	static int getmoon();

	// Extract the year component.
	static int getyear(const tm* atm);
	static int getyear(time_t curr);
	static int getyear();

	// Extract the weekday component.
	static int getweekday(const tm* atm);
	static int getweekday(time_t curr);
	static int getweekday();

	// Extract the hour component.
	static int gethour(const tm* atm);
	static int gethour(time_t curr);
	static int gethour();

	// Extract the minute component.
	static int getmin(const tm* atm);
	static int getmin(time_t curr);
	static int getmin();

	// Extract the second component.
	static int getsec(const tm* atm);
	static int getsec(time_t curr);
	static int getsec();

	// Extract weekday/hour/minute components together.
	static void getweekday(time_t curr, int& weekday, int& hour, int& minute);

	// Convert time_t into a broken-down local tm.
	static void gettm(time_t curr, tm& atm);

	// Return the next timestamp at or after `utc` with the requested hour/minute.
	static int getutcbyhour(time_t utc, int hour);
	static int getutcbymin(time_t utc, int amin);

	static bool checkutc(time_t autc1, time_t autc2);
	static bool checkutc(time_t autc);

	static bool issameweek(time_t timestamp1, time_t timestamp2);

	void test_isweek();
}// namespace ngl
