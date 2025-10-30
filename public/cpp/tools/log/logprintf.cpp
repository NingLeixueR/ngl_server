/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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
		if (em<ELOG_TYPE>::empty())
		{
			em<ELOG_TYPE>::set(em_pram(ELOG_DEFAULT));
			em<ELOG_TYPE>::set(em_pram(ELOG_BI));
		}
		create();
	}

	void logfile::flush()
	{
		m_stream.flush();
	}

	bool logfile::check_count()const
	{
		return m_count >= sysconfig::logline();
	}

	void logfile::close_fstream()
	{
		m_stream.close();
	}

	bool logfile::create_directories(const std::string& apath)
	{
		if (tools::directories_exists(apath) == false)
		{
			if (tools::create_directories(apath) == false)
			{
				return false;
			}
		}
		return true;
	}

	void logfile::create()
	{	
		std::string lpath = std::format("./{}", m_config.m_dir);
		if (create_directories(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}

		const tab_servers* ltabserver = ttab_servers::instance().tab();
		if (ltabserver == nullptr)
		{
			Throw("ttab_servers::instance().tab() fail {}", nconfig::m_tid);
		}

		lpath = std::format("{}/{}", lpath, ltabserver->m_name);
		if (create_directories(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}

		lpath = std::format("{}/{}", lpath, em<ELOG_TYPE>::get_tolower_name(m_config.m_type));
		if (create_directories(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}

		std::string ltimestr = tools::time2str((int)localtime::gettime(), "%Y-%m-%d");
		lpath = std::format("{}/{}", lpath, ltimestr);
		if (create_directories(lpath) == false)
		{
			Throw("not create path {}", lpath);
		}

		std::cout << "log dir:[" << lpath << "]" << std::endl;

		std::string ldaytimestr = tools::time2str((int)localtime::gettime(), "%H-%M-%S");
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
		const tab_servers* tab = ttab_servers::instance().tab(nnodeid::tid(alog->m_serverid));
		m_stream << 
			std::format("[{}][name:{},serverid:{},tcount:{}]{}\n[\n{}\n]\n", ngl::localtime::time2str(alog->m_time, "%H:%M:%S"),
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
		switch (aconfig.m_type)
		{
		case ELOG_DEFAULT:
		{
			return std::make_shared<logfile_default>(aconfig);
		}
		case ELOG_BI:
		{
			return std::make_shared<logfile_bi>(aconfig);
		}
		default:
		{
			return nullptr;
		}
		}
	}
}// namespace ngl