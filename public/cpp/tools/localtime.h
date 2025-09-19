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
			MILLISECOND = 1000,

			MINUTES_SECOND		= 1 * 60,							// 一分钟多少秒
			MINUTES_MILLISECOND = MINUTES_SECOND * MILLISECOND,		// 一分钟多少毫秒

			HOUR_MINUTES		= 1 * 60,							// 一小时多少分钟
			HOUR_SECOND			= MINUTES_SECOND * HOUR_MINUTES,	// 一小时多少秒
			HOUR_MILLISECOND	= HOUR_SECOND * MILLISECOND,		// 一小时多少毫秒

			DAY_HOUR			= 24,								// 一天多少小时
			DAY_MINUTES			= DAY_HOUR * HOUR_MINUTES,			// 一天多少分钟
			DAY_SECOND			= DAY_HOUR * HOUR_SECOND,			// 一天多少秒
			DAY_MILLISECOND		= DAY_SECOND * MILLISECOND,			// 一天多少毫秒

			WEEK_DAY			= 7,								// 一周多少天
			WEEK_HOUR			= WEEK_DAY * DAY_HOUR,				// 一周多少小时
			WEEK_MINUTES		= WEEK_DAY * DAY_MINUTES,			// 一周多少分钟
			WEEK_SECOND			= WEEK_DAY * DAY_SECOND,			// 一周多少秒
			WEEK_MILLISECOND	= WEEK_SECOND * MILLISECOND,		// 一周多少毫秒
		};
		// # 检查月份
		static bool check_month(int amonth/*1-12*/);

		static bool check_monthday(int amonthday/*1-31*/);

		static bool check_week(int aweek/*1-7*/);

		// # 检查小时
		static bool check_hour(int hour/*0-23*/);

		// # 检查分钟
		static bool check_minute(int minute/*0-59*/);

		// # 检查秒
		static bool check_sec(int sec/*0-59*/);

		// # 设置时间
		static bool settime(time_t sti);

		// # 获取时间
		static time_t gettime();

		// # 获取时间
		static time_t getsystime();

		// # 获取秒与秒之间的毫秒数 
		static time_t getms();

		// # 获取毫秒
		static time_t gettimems();

		// # 两个utc时间是否是同一天
		static bool issameday(time_t a1, time_t a2);

		// # 当前时间和参数utc时间是否是同一天
		static bool issameday(time_t autc);

		// # 打印时间
		static void printf_time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");

		// # utc转换为字符串
		static size_t time2str(char* str, int len, time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
		static std::string time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
		static std::string time2str(const char* format = "%Y-%m-%d %H:%M:%S");

		// # 字符串转换为utc
		static time_t str2time(const char* astr, const char* format = "%Y-%m-%d %H:%M:%S");

		// # 获取utc时刻那天的 小时分钟的utc
		static time_t getsecond2time(time_t utc, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # 获取今天的指定 小时分钟的utc(如果时间已过则获取下一天的指定小时分钟的utc)
		static time_t getsecond2time(int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # 获取指定分钟的utc(如果当前小时分钟秒数已过则获取下一个小时的)
		static time_t getsecond2time(int minute/*0-59*/, int sec/*0-59*/);

		// # 获取指定秒钟的utc(如果当前小时分钟秒数已过则获取下一个小时的)
		static time_t getsecond2time(int sec/*0-59*/);

		// # 获取指定星期的小时分钟
		static time_t getweekday(time_t utc, int aweek/*1-7*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # 获取本周几的指定 小时分钟的utc(如果时间已过则获取下一周几的指定小时分钟的utc) 
		static time_t getweekday(int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # 是否是闰年
		static bool isleapyear(int year);

		// # 是否存在年月日
		static bool mothday(int year, int month, int aday);

		// # 获取指定utc所在月份的日时分秒
		static std::pair<bool,time_t> getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # 获取本月 指定 日小时分钟的utc
		static std::pair<bool, time_t> getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # 获取两个时间之间相差的天数
		static time_t getspandays(time_t curr, time_t last);

		// # 当月的几号
		static int getmoonday(const tm* atm);
		static int getmoonday(time_t curr);
		static int getmoonday();

		// # 月份
		static int getmoon(const tm* atm);
		static int getmoon(time_t curr);
		static int getmoon();

		// # 年
		static int getyear(const tm* atm);
		static int getyear(time_t curr);
		static int getyear();

		// # 周:日
		static int getweekday(const tm* atm);
		static int getweekday(time_t curr);
		static int getweekday();

		// # 小时
		static int gethour(const tm* atm);
		static int gethour(time_t curr);
		static int gethour();

		// # 分钟
		static int getmin(const tm* atm);
		static int getmin(time_t curr);
		static int getmin();

		// # 秒数
		static int getsec(const tm* atm);
		static int getsec(time_t curr);
		static int getsec();

		// # 获取周: 日 时 分
		static void getweekday(time_t curr, int& weekday, int& hour, int& minute);

		// # 获取tm
		static void gettm(time_t curr, tm& atm);

		// # 获取指定utc后几个小时
		static int getutcbyhour(time_t utc, int hour);
		static int getutcbymin(time_t utc, int amin);

		static bool checkutc(time_t autc1, time_t autc2);
		static bool checkutc(time_t autc);

		static bool issameweek(time_t timestamp1, time_t timestamp2);
	private:
		static time_t m_offset;		//时间偏移量
	};

	void test_isweek();
}// namespace ngl