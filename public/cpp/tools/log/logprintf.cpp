#ifdef WIN32
# include <Windows.h>
#endif
#include <filesystem>
#include <iostream>
#include <format>

#include "ttab_servers.h"
#include "time_wheel.h"
#include "logprintf.h"
#include "localtime.h"
#include "enum2name.h"
#include "nprotocol.h"
#include "xmlinfo.h"
#include "nlog.h"
#include "xml.h"

namespace ngl
{
	time_wheel m_logwheel;

	void logprintf::printf(ELOGLEVEL acolor, const char* apos, const char* atimestr, const char* amsg)
	{
		switch (acolor)
		{
		case ELOG_DEBUG:
		{
#ifdef WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#else
			::printf("\033[32m");
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
			::printf("\033[0m");
#endif
		}
		break;
		case ELOG_INFO:
		{
#ifdef WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#else
			::printf("\033[32m");
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
			::printf("\033[0m");
#endif
		}
		break;
		case ELOG_WARN:
		{
#ifdef WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#else
			::printf("\033[33m");
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
			::printf("\033[0m");
#endif
		}
		break;
		case ELOG_ERROR:
		{
#ifdef WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#else
			::printf("\033[31m");
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
			::printf("\033[0m");
#endif
		}
		break;
		}
		::printf("\n");
	}

	logfile::logfile(bool aisactor, const config& aconfig) :
		m_config(aconfig),
		m_isactor(aisactor)
	{
		create();

		wheel_parm lparm
		{
			.m_ms = m_config.m_flush_time,
			.m_intervalms = [this](int64_t) {return m_config.m_flush_time; } ,
			.m_count = 0x7fffffff,
			.m_fun = [this](const wheel_node*)
			{
				m_stream.flush();
			}
		};
		m_logwheel.addtimer(lparm);
	}

	bool file_exists(const std::string& apath)
	{
		if (!std::filesystem::exists(apath))
		{
			return std::filesystem::create_directories(apath);
		}
		return true;
	}

	bool logfile::check_count()const
	{
		return m_count >= sysconfig::logline();
	}

	void logfile::close_fstream()
	{
		m_stream.close();
	}

	void logfile::create()
	{	
		std::string lpath("./");
		lpath += m_config.m_dir;
		if (file_exists(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}
		
		lpath += '/';
		lpath += m_isactor ? "net" : "local";

		if (file_exists(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}
		lpath += '/';

		std::cout << "log dir:[" << lpath << "]" << std::endl;

		std::string ltimestr = tools::time2str((int)localtime::gettime(), "%Y%m%d_%H%M%S");
		std::string lopfile = std::format("{}/{}_{}.log", lpath, ltimestr, m_fcount);
		++m_fcount;
		
		m_stream.open(lopfile, std::ios::binary);
		m_count = 0;
	}

	struct logfile_default : public logfile
	{
		logfile_default(bool aisactor, const config& aconfig);
		void printf(const logitem* alog) final;
	};

	logfile_default::logfile_default(bool aisactor, const config& aconfig) :
		logfile(aisactor, aconfig)
	{}

	void logfile_default::printf(const logitem* alog)
	{
		const tab_servers* tab = ttab_servers::tab(alog->m_serverid);
		m_stream << 
			std::format("[{}][name:{},serverid:{},tcount:{}][{}:{}]", ngl::localtime::time2str(alog->m_time, "%Y/%m/%d %H:%M:%S"), 
				tab->m_name, tab->m_id, tab->m_tcount, alog->m_src, alog->m_data
			)<< std::endl;
		++m_count;
		if (check_count())
		{
			close_fstream();
			create();
		}
	}

	// ### BI
	struct logfile_bi : public logfile
	{
		logfile_bi(bool aisactor, const config& aconfig);
		void printf(const logitem* alog) final;
	};

	logfile_bi::logfile_bi(bool aisactor, const config& aconfig) :
		logfile(aisactor, aconfig)
	{}

	void logfile_bi::printf(const logitem* alog)
	{
		m_stream << alog->m_data << std::endl;
	}

	std::shared_ptr<logfile> logfile::create_make(bool aisactor, const config& aconfig)
	{
		switch (aconfig.m_type)
		{
		case ELOG_LOCAL:
			return std::make_shared<logfile_default>(aisactor, aconfig);
		case ELOG_NETWORK:
			return std::make_shared<logfile_default>(aisactor, aconfig);
		case ELOG_BI:
			return std::make_shared<logfile_bi>(aisactor, aconfig); 
		default:
			return nullptr;
		}
	}
}// namespace ngl