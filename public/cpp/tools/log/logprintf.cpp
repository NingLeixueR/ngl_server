#ifdef WIN32
# include <Windows.h>
#endif

#include <filesystem>
#include <iostream>

#include "ttab_servers.h"
#include "time_wheel.h"
#include "logprintf.h"
#include "localtime.h"
#include "enum2name.h"
#include "logformat.h"
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
		create(true);

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

	void logfile::create(bool afirst)
	{
		if (afirst == false)
		{
			if (m_count < DEF_LOG_MAX_LINE)
				return;
		}
		
		if (m_stream.is_open())
			m_stream.close();

		std::string lpath("./");
		lpath += m_config.m_dir;
		lpath += '/';
		lpath += m_isactor ? "net/" : "local/";
		//lpath += nconfig::m_nodename;
		std::cout<< "log dir:[" << lpath << "]" << std::endl;
			
		if (afirst)
		{//检查目录是否存在
			if (!std::filesystem::exists(lpath))
			{
				bool ret = std::filesystem::create_directories(lpath);
				if (!ret)
				{
					throw "not create path" + lpath;
				}
			}
		}

		char lbufftime[1024] = { 0 };
		localtime::time2str(lbufftime, 1024, time(nullptr), "%Y%m%d_%H%M%S");

		const char* lp = nullptr;
		switch (m_config.m_type)
		{
		case ELOG_LOCAL:
			lp = "local";
			break;
		case ELOG_NETWORK:
			lp = "network";
			break;
		case ELOG_BI:
			lp = "bi";
			break;
		default:
			lp = "";
			break;
		}
		char lbuff[1024];
		
		snprintf(lbuff, 1024, "/%d_%s_%s_%d.log", nconfig::m_nodeid, lp, lbufftime, ++m_fcount);
		
		m_stream.open(lpath + lbuff, std::ios::binary);
		//m_errorstream.
		m_count = 0;
	}

	struct logfile_default : public logfile
	{
		logfile_default(bool aisactor, const config& aconfig);
		virtual void printf(const logitem& alog);
		virtual void local_printf(ELOG atype, ngl::logformat& llogformat);
	};

	logfile_default::logfile_default(bool aisactor, const config& aconfig) :
		logfile(aisactor, aconfig)
	{}

	void logfile_default::printf(const logitem& alog)
	{
		tab_servers* tab = ttab_servers::tab(alog.m_serverid);
		m_stream << 
			"[server:" << tab->m_name << "#" << alog.m_serverid 
			<< "][" <<
			elog_name::get((ELOG)alog.m_type) 
			<< "][" << 
			alog.m_pos 
			<< "][" 
			<< alog.m_head << "]\t" << alog.m_str << std::endl;
		++m_count;
		create(false);
	}

	void logfile_default::local_printf(ELOG atype, ngl::logformat& llogformat)
	{
		m_stream << "[serverid:" << nconfig::m_nodeid << "][" << elog_name::get(atype) << "][" << llogformat.data("pos") << "][" << llogformat.data("head") << "]\t" << llogformat.data("src") << std::endl;
		++m_count;
		create(false);
	}

	// ### BI
	struct logfile_bi : public logfile
	{
		logfile_bi(bool aisactor, const config& aconfig);
		virtual void printf(const logitem& alog);
	};

	logfile_bi::logfile_bi(bool aisactor, const config& aconfig) :
		logfile(aisactor, aconfig)
	{}

	void logfile_bi::printf(const logitem& alog)
	{
		m_stream << alog.m_str << std::endl;
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