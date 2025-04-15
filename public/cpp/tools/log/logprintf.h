#pragma once

#include <fstream>
#include <memory>

#include "sysconfig.h"

namespace ngl
{
	enum ELOG_TYPE
	{
		ELOG_LOCAL		= 0,
		ELOG_NETWORK	= 1,
		ELOG_BI			= 2,
		ELOG_COUNT,
	};

	class logprintf
	{
	public:
		static void printf(ELOGLEVEL acolor, const char* apos, const char* atimestr, const char* amsg);
	};

	struct np_logitem;

	struct logfile
	{
		struct config
		{
			i32_actordataid m_id;			// nlogactor
			std::string		m_dir;			// �ļ���
			enum
			{
				min_flush_time = 1000,
			};
			int32_t			m_flush_time = 0;	// д���ļ��ļ��
			int32_t flush_time()
			{
				return std::max(m_flush_time, (int32_t)min_flush_time);
			}
		};
		std::ofstream	m_stream;
		config			m_config;
		int				m_count = 0;
		int				m_fcount = 0;

		logfile(const config& aconfig);

		bool check_count()const;

		void close_fstream();

		bool create_directories(const std::string& apath);

		void create();

		virtual void printf(const np_logitem* alog) = 0;

		void flush();

		static std::shared_ptr<logfile> create_make(const config& aconfig);
	};
}// namespace ngl