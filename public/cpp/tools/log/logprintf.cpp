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
#include "logformat.h"
#include "nprotocol.h"
#include "xmlinfo.h"
#include "xmlnode.h"
#include "nlog.h"

namespace ngl
{
	time_wheel m_logwheel;

	void logprintf::printf(ELOG acolor, const char* apos, const char* atimestr, const char* amsg)
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
		m_isactor(aisactor),
		m_config(aconfig),
		m_count(0),
		m_fcount(0)
	{
		create();

		wheel_parm lparm
		{
			.m_ms = m_config.m_flush_time,
			.m_intervalms = [this](int64_t) {return m_config.m_flush_time; } ,
			.m_count = 0x7fffffff,
			.m_fun = [this](wheel_node* anode)
			{
				m_stream.flush();
			}
		};
		m_logwheel.addtimer(lparm);
	}

	bool file_exists(std::string& apath)
	{
		if (!std::filesystem::exists(apath))
		{
			return std::filesystem::create_directories(apath);
		}
		return true;
	}

	bool logfile::check_count()
	{
		return m_count >= sysconfig::logline();
	}

	void logfile::close_fstream()
	{
		//if (m_stream.is_open())
		//{
			m_stream.close();
		//}
	}

	void logfile::create()
	{	
		std::string lpath("./");
		lpath += m_config.m_dir;
		if (file_exists(lpath) == false)
		{
			throw "not create path" + lpath;
		}
		
		lpath += '/';
		lpath += m_isactor ? "net" : "local";

		if (file_exists(lpath) == false)
		{
			throw "not create path" + lpath;
		}
		lpath += '/';

		std::cout << "log dir:[" << lpath << "]" << std::endl;

		char lbufftime[1024] = { 0 };
		localtime::time2str(lbufftime, 1024, time(nullptr), "%Y%m%d%H%M%S");

		char lbuff[1024];
		snprintf(lbuff, 1024, "/%s_%d.log", lbufftime, ++m_fcount);
		
		m_stream.open(lpath + lbuff, std::ios::binary);
		m_count = 0;
	}

	struct logfile_default : public logfile
	{
		logfile_default(bool aisactor, const config& aconfig);
		virtual void printf(const logitem* alog);
		virtual void local_printf(ELOG atype, ngl::logformat& llogformat);
	};

	logfile_default::logfile_default(bool aisactor, const config& aconfig) :
		logfile(aisactor, aconfig)
	{}

	void logfile_default::printf(const logitem* alog)
	{
		tab_servers* tab = ttab_servers::tab(alog->m_serverid);
		m_stream << "[" << tab->m_name << ":" << alog->m_serverid << "]";
		m_stream << "[" << alog->m_pos << "]";
		m_stream << "[" << alog->m_head << "]\t";
		m_stream << alog->m_str << std::endl;
		++m_count;
		if (check_count())
		{
			close_fstream();
			create();
		}
	}

	void logfile_default::local_printf(ELOG atype, ngl::logformat& llogformat)
	{
		m_stream 
			<< "[serid:" << nconfig::m_nodeid << "]"
			<< "[" << elog_name::get(atype) << "]"
			<< "[" << llogformat.data("pos") << "]"
			<< "[" << llogformat.data("head") << "]\t" 
			<< llogformat.data("src") << std::endl;
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
		virtual void printf(const logitem* alog);
	};

	logfile_bi::logfile_bi(bool aisactor, const config& aconfig) :
		logfile(aisactor, aconfig)
	{}

	void logfile_bi::printf(const logitem* alog)
	{
		m_stream << alog->m_str << std::endl;
	}

	std::shared_ptr<logfile> logfile::create_make(bool aisactor, const config& aconfig)
	{
		switch (aconfig.m_type)
		{
		case ELOG_LOCAL:
			return std::shared_ptr<logfile>(new logfile_default(aisactor, aconfig));
		case ELOG_NETWORK:
			return std::shared_ptr<logfile>(new logfile_default(aisactor, aconfig));
		case ELOG_BI:
			return std::shared_ptr<logfile>(new logfile_bi(aisactor, aconfig));
		}
		return nullptr;
	}
}// namespace ngl