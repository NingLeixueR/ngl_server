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
		// # ����·�
		static bool check_month(int amonth/*1-12*/);

		static bool check_monthday(int amonthday/*1-31*/);

		static bool check_week(int aweek/*1-7*/);

		// # ���Сʱ
		static bool check_hour(int hour/*0-23*/);

		// # ������
		static bool check_minute(int minute/*0-59*/);

		// # �����
		static bool check_sec(int sec/*0-59*/);

		// # ����ʱ��
		static bool settime(time_t sti);

		// # ��ȡʱ��
		static time_t gettime();

		// # ��ȡʱ��
		static time_t getsystime();

		// # ��ȡ������֮��ĺ����� 
		static time_t getms();

		// # ��ȡ����
		static time_t gettimems();

		// # ����utcʱ���Ƿ���ͬһ��
		static bool issameday(time_t a1, time_t a2);

		// # ��ǰʱ��Ͳ���utcʱ���Ƿ���ͬһ��
		static bool issameday(time_t autc);

		// # ��ӡʱ��
		static void printf_time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");

		// # utcת��Ϊ�ַ���
		static size_t time2str(char* str, int len, time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
		static std::string time2str(time_t anow, const char* format = "%Y-%m-%d %H:%M:%S");
		static std::string time2str(const char* format = "%Y-%m-%d %H:%M:%S");

		// # �ַ���ת��Ϊutc
		static time_t str2time(const char* astr, const char* format = "%Y-%m-%d %H:%M:%S");

		// # ��ȡutcʱ������� Сʱ���ӵ�utc
		static time_t getsecond2time(time_t utc, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # ��ȡ�����ָ�� Сʱ���ӵ�utc(���ʱ���ѹ����ȡ��һ���ָ��Сʱ���ӵ�utc)
		static time_t getsecond2time(int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # ��ȡָ�����ӵ�utc(�����ǰСʱ���������ѹ����ȡ��һ��Сʱ��)
		static time_t getsecond2time(int minute/*0-59*/, int sec/*0-59*/);

		// # ��ȡָ�����ӵ�utc(�����ǰСʱ���������ѹ����ȡ��һ��Сʱ��)
		static time_t getsecond2time(int sec/*0-59*/);

		// # ��ȡָ�����ڵ�Сʱ����
		static time_t getweekday(time_t utc, int aweek/*1-7*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # ��ȡ���ܼ���ָ�� Сʱ���ӵ�utc(���ʱ���ѹ����ȡ��һ�ܼ���ָ��Сʱ���ӵ�utc) 
		static time_t getweekday(int aweek/*0-6*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # �Ƿ�������
		static bool isleapyear(int year);

		// # �Ƿ����������
		static bool mothday(int year, int month, int aday);

		// # ��ȡָ��utc�����·ݵ���ʱ����
		static std::pair<bool,time_t> getmothday(time_t utc, int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # ��ȡ���� ָ�� ��Сʱ���ӵ�utc
		static std::pair<bool, time_t> getmothday(int amday/*1-31*/, int hour/*0-23*/, int minute/*0-59*/, int sec/*0-59*/);

		// # ��ȡ����ʱ��֮����������
		static time_t getspandays(time_t curr, time_t last);

		// # ���µļ���
		static int getmoonday(const tm* atm);
		static int getmoonday(time_t curr);
		static int getmoonday();

		// # �·�
		static int getmoon(const tm* atm);
		static int getmoon(time_t curr);
		static int getmoon();

		// # ��
		static int getyear(const tm* atm);
		static int getyear(time_t curr);
		static int getyear();

		// # ��:��
		static int getweekday(const tm* atm);
		static int getweekday(time_t curr);
		static int getweekday();

		// # Сʱ
		static int gethour(const tm* atm);
		static int gethour(time_t curr);
		static int gethour();

		// # ����
		static int getmin(const tm* atm);
		static int getmin(time_t curr);
		static int getmin();

		// # ����
		static int getsec(const tm* atm);
		static int getsec(time_t curr);
		static int getsec();

		// # ��ȡ��: �� ʱ ��
		static void getweekday(time_t curr, int& weekday, int& hour, int& minute);

		// # ��ȡtm
		static void gettm(time_t curr, tm& atm);

		// # ��ȡָ��utc�󼸸�Сʱ
		static int getutcbyhour(time_t utc, int hour);
		static int getutcbymin(time_t utc, int amin);

		static bool checkutc(time_t autc1, time_t autc2);
		static bool checkutc(time_t autc);

		static bool issameweek(time_t timestamp1, time_t timestamp2);
	private:
		static time_t m_offset;		//ʱ��ƫ����
	};

	void test_isweek();
}// namespace ngl