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

namespace ngl::tools::time
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
	bool check_month(int amonth/*1-12*/);

	// Validate a day-of-month value.
	bool check_monthday(int amonthday/*1-31*/);

	// Validate a weekday value using the project's 1-7 convention.
	bool check_week(int aweek/*1-7*/);

	// Validate an hour-of-day value.
	bool check_hour(int hour/*0-23*/);

	// Validate a minute value.
	bool check_minute(int minute/*0-59*/);

	// Validate a second value.
	bool check_sec(int sec/*0-59*/);

	// Return whether the elapsed wall-clock time exceeds atimeoutms.
	bool timeout(time_t abeg, int32_t atimeoutms);

	// Apply a positive offset so tools::time::gettime() can follow a server-synchronized clock.
	bool settime(time_t sti);

	// Return logical server time, raw system time, milliseconds, and millisecond remainder.
	time_t gettime();

	// Return raw system time in seconds without the logical offset.
	time_t getsystime();

	// Return raw system time in milliseconds.
	time_t getms();

	// Return logical server time in milliseconds.
	time_t gettimems();

	// Date-comparison helpers.
	bool issameday(time_t a1, time_t a2);

	// Compare the current logical day against the supplied timestamp.
	bool issameday(time_t autc);

	// Format time_t values into printable strings.
	void printf_time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");

	// Write a formatted time string into a caller-provided buffer.
	size_t time2str(char* str, int len, time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
	std::string time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
	std::string time2str(const char* format = "%Y-%m-%d %H:%M:%S");

	// Parse formatted local time strings back into time_t.
	time_t str2time(const char* astr, const char* format = "%Y-%m-%d %H:%M:%S");

	// Build the timestamp for a specific time-of-day on a given day or the next matching day.
	time_t getsecond2time(time_t utc, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Build the next matching timestamp from the current logical day.
	time_t getsecond2time(int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Build the next matching timestamp from the current logical hour.
	time_t getsecond2time(int minute/*0-59*/, int sec/*0-59*/);

	// Build the next matching timestamp from the current logical minute.
	time_t getsecond2time(int sec/*0-59*/);

	// Build timestamps for specific weekday/month-day schedules.
	time_t getweekday(time_t utc, int aweek/*1-7*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Build the next timestamp for the requested weekday and time-of-day.
	time_t getweekday(int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Calendar helpers.
	bool isleapyear(int year);

	// Return whether the supplied year/month/day triple is a valid date.
	bool mothday(int year, int month, int aday);

	// Build the next timestamp for the requested day-of-month and time-of-day.
	std::pair<bool, time_t> getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Convenience overload that uses the current logical time as the base.
	std::pair<bool, time_t> getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

	// Return whole-day difference between two timestamps after truncating to midnight.
	time_t getspandays(time_t curr, time_t last);

	// Extract the day-of-month component.
	int getmoonday(const tm* atm);
	int getmoonday(time_t curr);
	int getmoonday();

	// Extract the month component.
	int getmoon(const tm* atm);
	int getmoon(time_t curr);
	int getmoon();

	// Extract the year component.
	int getyear(const tm* atm);
	int getyear(time_t curr);
	int getyear();

	// Extract the weekday component.
	int getweekday(const tm* atm);
	int getweekday(time_t curr);
	int getweekday();

	// Extract the hour component.
	int gethour(const tm* atm);
	int gethour(time_t curr);
	int gethour();

	// Extract the minute component.
	int getmin(const tm* atm);
	int getmin(time_t curr);
	int getmin();

	// Extract the second component.
	int getsec(const tm* atm);
	int getsec(time_t curr);
	int getsec();

	// Extract weekday/hour/minute components together.
	void getweekday(time_t curr, int& weekday, int& hour, int& minute);

	// Convert time_t into a broken-down local tm.
	void gettm(time_t curr, tm& atm);

	// Return the next timestamp at or after `utc` with the requested hour/minute.
	int getutcbyhour(time_t utc, int hour);
	int getutcbymin(time_t utc, int amin);

	bool checkutc(time_t autc1, time_t autc2);
	bool checkutc(time_t autc);

	bool issameweek(time_t timestamp1, time_t timestamp2);

	void test_isweek();
}// namespace ngl::tools::time
