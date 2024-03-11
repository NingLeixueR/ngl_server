#pragma once

#include <boost/date_time/c_time.hpp>
#include <cstdio>
#include <ctime>

namespace ngl
{
	class localtime
	{
	public:
		enum
		{
			MILLISECOND = 1000,

			DAY_HOUR = 24,								// 一天多少小时
			DAY_MINUTES = DAY_HOUR * 60,				// 一天多少分钟
			DAY_SECOND = DAY_MINUTES * 60,				// 一天多少秒
			DAY_MILLISECOND = DAY_SECOND * MILLISECOND,	// 一天多少毫秒

			WEEK_DAY = 7,								// 一周多少天
			WEEK_HOUR = WEEK_DAY * DAY_HOUR,			// 一周多少小时
			WEEK_MINUTES = WEEK_DAY * DAY_MINUTES,		// 一周多少分钟
			WEEK_SECOND = WEEK_DAY * DAY_SECOND,		// 一周多少秒
			WEEK_MILLISECOND = WEEK_SECOND * MILLISECOND,	// 一周多少毫秒

			HOUR_MINUTES = 1 * 60,							// 一小时多少分钟
			HOUR_SECOND = HOUR_MINUTES * 60,				// 一小时多少秒
			HOUR_MILLISECOND = HOUR_SECOND * MILLISECOND,	// 一小时多少毫秒

			MINUTES_SECOND = 1 * 60,							// 一分钟多少秒
			MINUTES_MILLISECOND = MINUTES_SECOND * MILLISECOND,	// 一分钟多少毫秒
		};

		/** 设置时间 */
		static bool settime(time_t sti);
		/** 获取时间*/
		static time_t gettime();
		/** 获取秒与秒之间的毫秒数 */
		static time_t getms();
		/** 获取毫秒 */
		static time_t gettimems();
		static void printf_time2str(time_t anow, const char* format);
		//Year-Month-Day Hour:Minuts:Second %y-%m-%d %H:%M:%S
		static size_t time2str(char* str, int len, time_t anow, const char* format);
		static size_t time2msstr(char* str, int len, time_t anow, const char* format);
		static std::string time2msstr(time_t anow, const char* format);
		static std::string time2msstr(const char* format);
		//static const char* time2str(int anow, const char* format, char* abuff);
		static time_t st2time(const char* str);
		// 获取utc时刻那天的 小时分钟的utc
		static time_t getsecond2time(time_t utc, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);
		// 获取今天的指定 小时分钟的utc(如果时间已过则获取下一天的指定小时分钟的utc)
		static time_t getsecond2time(int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);
		// 获取指定分钟的utc(如果当前小时分钟秒数已过则获取下一个小时的)
		static time_t getsecond2time(int minute/*0-59*/, int sec/*0-59*/);
		// 获取指定秒钟的utc(如果当前小时分钟秒数已过则获取下一个小时的)
		static time_t getsecond2time(int sec/*0-59*/);

		static time_t getweekday(time_t utc, int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);
		// 获取本周几的指定 小时分钟的utc(如果时间已过则获取下一周几的指定小时分钟的utc) 
		static time_t getweekday(int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		static time_t getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);
		// 获取本月 指定 日小时分钟的utc(如果时间已过则获取下一月的指定日小时分钟的utc) 
		static time_t getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		//获取两个时间之间相差的天数
		static time_t getspandays(time_t curr, time_t last);
		static time_t getnextweekdaytime(time_t curr, time_t weekday);
		static time_t getnextweekdaytime(time_t curr, int weekday, int hour, int minute);
		static bool isspanweekday(time_t curr, time_t last, int weekday);
		static bool isspanweekday(time_t curr, time_t last, int weekday, int hour, int minute);
		//获得两个时间之间相差多少周
		static time_t getspanweeks(time_t curr, time_t last, int weekday);
		//获取指定小时，分钟时间和目前时间的差值，返回负数则表示指定时间已过 */
		static time_t getdesttimevalue(int hour, int minute, int sec);
		//获取utc那天的第x天的0点的utc时间，utc当天算第0天，utc明天算第1天
		static time_t getutcbyday(time_t utc, int dayNum);
		//获取指定年，月，日，小时，分钟时间和目前时间的差值，返回负数则表示指定时间已过 */
		static time_t getdestdayvalue(time_t curr, int year, int month, int day, int hour, int minute);
		// 当月的几号
		static int getmoonday(const tm* atm);
		static int getmoonday(time_t curr);
		static int getmoonday();
		static int getmoon(const tm* atm);
		static int getmoon(time_t curr);
		static int getmoon();
		static int getyear(const tm* atm);
		static int getyear(time_t curr);
		static int getyear();
		static int getweekday(const tm* atm);
		static int getweekday(time_t curr);
		static int getweekday();
		static int gethour(const tm* atm);
		static int gethour(time_t curr);
		static int gethour();
		static int getmin(const tm* atm);
		static int getmin(time_t curr);
		static int getmin();
		static int getsec(const tm* atm);
		static int getsec(time_t curr);
		static int getsec();
		static void getweekday(time_t curr, int& weekday, int& hour, int& minute);
		static void gettm(time_t curr, tm& atm);
		// 获取指定utc后几个小时
		static int getutcbyhour(time_t utc, int hour);
		static int getutcbymin(time_t utc, int min);
	private:
		static time_t m_offset;		//时间偏移量
	};
}// namespace ngl