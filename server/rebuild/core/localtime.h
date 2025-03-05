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

			MINUTES_SECOND		= 1 * 60,							// һ���Ӷ�����
			MINUTES_MILLISECOND = MINUTES_SECOND * MILLISECOND,		// һ���Ӷ��ٺ���

			HOUR_MINUTES		= 1 * 60,							// һСʱ���ٷ���
			HOUR_SECOND			= MINUTES_SECOND * HOUR_MINUTES,	// һСʱ������
			HOUR_MILLISECOND	= HOUR_SECOND * MILLISECOND,		// һСʱ���ٺ���

			DAY_HOUR			= 24,								// һ�����Сʱ
			DAY_MINUTES			= DAY_HOUR * HOUR_MINUTES,			// һ����ٷ���
			DAY_SECOND			= DAY_HOUR * HOUR_SECOND,			// һ�������
			DAY_MILLISECOND		= DAY_SECOND * MILLISECOND,			// һ����ٺ���

			WEEK_DAY			= 7,								// һ�ܶ�����
			WEEK_HOUR			= WEEK_DAY * DAY_HOUR,				// һ�ܶ���Сʱ
			WEEK_MINUTES		= WEEK_DAY * DAY_MINUTES,			// һ�ܶ��ٷ���
			WEEK_SECOND			= WEEK_DAY * DAY_SECOND,			// һ�ܶ�����
			WEEK_MILLISECOND	= WEEK_SECOND * MILLISECOND,		// һ�ܶ��ٺ���
		};

		/** ����ʱ�� */
		static bool settime(time_t sti);
		/** ��ȡʱ��*/
		static time_t gettime();
		/** ��ȡ������֮��ĺ����� */
		static time_t getms();
		/** ��ȡ���� */
		static time_t gettimems();
		static void printf_time2str(time_t anow, const char* format);
		//Year-Month-Day Hour:Minuts:Second %y-%m-%d %H:%M:%S
		static size_t time2str(char* str, int len, time_t anow, const char* format);
		static std::string time2str(time_t anow, const char* format);
		static std::string time2str(const char* format);
		static time_t str2time(const char* astr, const char* format = "%Y-%m-%d %H:%M:%S");
		// ��ȡutcʱ������� Сʱ���ӵ�utc
		static time_t getsecond2time(time_t utc, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);
		// ��ȡ�����ָ�� Сʱ���ӵ�utc(���ʱ���ѹ����ȡ��һ���ָ��Сʱ���ӵ�utc)
		static time_t getsecond2time(int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);
		// ��ȡָ�����ӵ�utc(�����ǰСʱ���������ѹ����ȡ��һ��Сʱ��)
		static time_t getsecond2time(int minute/*0-59*/, int sec/*0-59*/);
		// ��ȡָ�����ӵ�utc(�����ǰСʱ���������ѹ����ȡ��һ��Сʱ��)
		static time_t getsecond2time(int sec/*0-59*/);

		static time_t getweekday(time_t utc, int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);
		// ��ȡ���ܼ���ָ�� Сʱ���ӵ�utc(���ʱ���ѹ����ȡ��һ�ܼ���ָ��Сʱ���ӵ�utc) 
		static time_t getweekday(int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		static time_t getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);
		// ��ȡ���� ָ�� ��Сʱ���ӵ�utc(���ʱ���ѹ����ȡ��һ�µ�ָ����Сʱ���ӵ�utc) 
		static time_t getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		//��ȡ����ʱ��֮����������
		static time_t getspandays(time_t curr, time_t last);
		static time_t getnextweekdaytime(time_t curr, time_t weekday);
		static time_t getnextweekdaytime(time_t curr, int weekday, int hour, int minute);
		static bool isspanweekday(time_t curr, time_t last, int weekday);
		static bool isspanweekday(time_t curr, time_t last, int weekday, int hour, int minute);
		//�������ʱ��֮����������
		static time_t getspanweeks(time_t curr, time_t last, int weekday);
		//��ȡָ��Сʱ������ʱ���Ŀǰʱ��Ĳ�ֵ�����ظ������ʾָ��ʱ���ѹ� */
		static time_t getdesttimevalue(int hour, int minute, int sec);
		//��ȡutc����ĵ�x���0���utcʱ�䣬utc�������0�죬utc�������1��
		static time_t getutcbyday(time_t utc, int dayNum);
		//��ȡָ���꣬�£��գ�Сʱ������ʱ���Ŀǰʱ��Ĳ�ֵ�����ظ������ʾָ��ʱ���ѹ� */
		static time_t getdestdayvalue(time_t curr, int year, int month, int day, int hour, int minute);
		// ���µļ���
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
		// ��ȡָ��utc�󼸸�Сʱ
		static int getutcbyhour(time_t utc, int hour);
		static int getutcbymin(time_t utc, int min);

		static bool issameweek(time_t timestamp1, time_t timestamp2);
	private:
		static time_t m_offset;		//ʱ��ƫ����
	};
}// namespace ngl