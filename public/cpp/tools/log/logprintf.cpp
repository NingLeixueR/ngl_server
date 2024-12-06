#ifdef WIN32
# include <Windows.h>
#endif
#include <filesystem>
#include <iostream>
#include <format>

#include "ttab_servers.h"
#include "time_wheel.h"
#include "actor_log.h"
#include "logprintf.h"
#include "localtime.h"
#include "enum2name.h"
#include "nprotocol.h"
#include "xmlinfo.h"
#include "actor.h"
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

	logfile::logfile(const config& aconfig) :
		m_config(aconfig)
	{
		create();

		int32_t lflushtime = m_config.flush_time();
		wheel_parm lparm
		{
			.m_ms = lflushtime,
			.m_intervalms = [lflushtime](int64_t) {return lflushtime; } ,
			.m_count = 0x7fffffff,
			.m_fun = [this](const wheel_node*)
			{
				auto pro = std::make_shared<np_logflush>();
				actor::static_send_actor(actor_log::actorid(nlogactor::actor_type(m_config.m_id), nlogactor::log_type(m_config.m_id)), nguid::make(), pro);
			}
		};
		m_logwheel.addtimer(lparm);
	}

	void logfile::flush()
	{
		m_stream.flush();
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

	std::string& elog_name(ELOG_TYPE aactortype)
	{
		static std::string llocal = "local";
		static std::string lnetwork = "network";
		static std::string lbi = "bi";
		static std::string lnone = "none";
		switch (aactortype)
		{
		case ELOG_LOCAL:
			return llocal;
		case ELOG_NETWORK:
			return lnetwork;
		case ELOG_BI:
			return lbi;
		}
		return lnone;
	}

	void logfile::create()
	{	
		std::string lpath = std::format("./{}", m_config.m_dir);
		if (file_exists(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}

		ENUM_ACTOR lactortype = nlogactor::actor_type(m_config.m_id);
		const char* lname = (lactortype == ACTOR_NONE) ? "sys_global" : em<ENUM_ACTOR>::get_tolower_name(lactortype);
		lpath = std::format("{}/{}", lpath, lname);
		if (file_exists(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}

		std::string ltimestr = tools::time2str((int)localtime::gettime(), "%Y%m%d");
		lpath = std::format("{}/{}", lpath, ltimestr);
		if (file_exists(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}

		std::string lelogname = elog_name(nlogactor::log_type(m_config.m_id));
		lpath = std::format("{}/{}", lpath, lelogname);
		if (file_exists(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}

		std::cout << "log dir:[" << lpath << "]" << std::endl;

		std::string ldaytimestr = tools::time2str((int)localtime::gettime(), "%H%M%S");
		std::string lopfile = std::format("{}/{}_{}.log", lpath, ldaytimestr, m_fcount);
		++m_fcount;
		
		m_stream = std::ofstream();
		m_stream.open(lopfile, std::ios::binary);
		m_count = 0;
	}

	struct logfile_default : public logfile
	{
		logfile_default(const config& aconfig);
		void printf(const np_logitem* alog) final;
	};

	logfile_default::logfile_default(const config& aconfig) :
		logfile(aconfig)
	{}

	void logfile_default::printf(const np_logitem* alog)
	{
		const tab_servers* tab = ttab_servers::tab(alog->m_serverid);
		m_stream << 
			std::format("[{}][name:{},serverid:{},tcount:{}]{}\n#\n{}\n#\n", ngl::localtime::time2str(alog->m_time, "%H:%M:%S"), 
				tab->m_name, tab->m_id, tab->m_tcount, alog->m_src, alog->m_data
			);
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
		logfile_bi(const config& aconfig);
		void printf(const np_logitem* alog) final;
	};

	logfile_bi::logfile_bi(const config& aconfig) :
		logfile(aconfig)
	{}

	void logfile_bi::printf(const np_logitem* alog)
	{
		m_stream << alog->m_data << std::endl;
	}

	std::shared_ptr<logfile> logfile::create_make(const config& aconfig)
	{
		switch (nlogactor::log_type(aconfig.m_id))
		{
		case ELOG_LOCAL:
			return std::make_shared<logfile_default>(aconfig);
		case ELOG_NETWORK:
			return std::make_shared<logfile_default>(aconfig);
		case ELOG_BI:
			return std::make_shared<logfile_bi>(aconfig);
		default:
			return nullptr;
		}
	}
}// namespace ngl