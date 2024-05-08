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
		static void printf(ELOG acolor, const char* apos, const char* atimestr, const char* amsg);
	};

	struct logitem;

	struct logfile
	{
		struct config
		{
			ELOG_TYPE	m_type;			// 日志类型 ELOG_TYPE
			std::string m_dir;			// 文件夹
			int			m_flush_time;	// 写入文件的间隔
		};
		std::ofstream	m_stream;
		config			m_config;
		int				m_count;
		int				m_fcount;
		bool			m_isactor;

		logfile(bool aisactor, const config& aconfig);

		bool check_count();

		void close_fstream();

		void create();

		virtual void printf(const logitem* alog) = 0;

		virtual void local_printf(ELOG atype, ngl::logformat& llogformat) {}

		static std::shared_ptr<logfile> create_make(bool aisactor, const config& aconfig);
	};
}// namespace ngl