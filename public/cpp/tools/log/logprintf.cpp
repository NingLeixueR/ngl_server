/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for log.


#include "actor/actor_logic/actor_log/actor_log.h"
#include "actor/actor_base/core/actor.h"
#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_servers.h"
#include "tools/tools/tools_enum.h"
#include "tools/tab/xml/xmlinfo.h"
#include "tools/log/logprintf.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools/tools_time_wheel.h"
#include "tools/tools/tools_time.h"
#include "tools/log/nlog.h"

#ifdef WIN32
# include <Windows.h>
#endif
#include <filesystem>
#include <iostream>
#include <format>

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
		default:
		{
			::printf("[%s][%s][%s]	%s\n", atimestr, apos, elog_name::get(acolor), amsg);
		}
		break;
		}
		::printf("\n");
	}

	logfile::logfile(const config& aconfig) :
		m_config(aconfig)
	{
		if (tools::em<ELOG_TYPE>::empty())
		{
			tools::em<ELOG_TYPE>::set(em_pram(ELOG_DEFAULT));
			tools::em<ELOG_TYPE>::set(em_pram(ELOG_BI));
		}
		create();
	}

	void logfile::flush()
	{
		m_stream.flush();
	}

	bool logfile::flush_count()const
	{
		return m_count >= sysconfig::logline();
	}

	void logfile::close_fstream()
	{
		m_stream.close();
	}

	bool logfile::create_dir(const std::string& apath)
	{
		if (tools::directories_exists(apath) == false)
		{
			if (tools::create_dir(apath) == false)
			{
				return false;
			}
		}
		return true;
	}

	void logfile::create()
	{	
		// Logs are grouped by server name, stream type, and date so rotation
		// stays human-readable even when multiple nodes run on one machine.
		std::string lpath = std::format("./{}", m_config.m_dir);
		if (create_dir(lpath) == false)
		{
			tools::no_core_dump();
			return;
		}

		const tab_servers* tab = ttab_servers::instance().const_tab();
		if (tab == nullptr)
		{
			tools::no_core_dump();
			return;
		}

		lpath = std::format("{}/{}", lpath, tab->m_name);
		if (create_dir(lpath) == false)
		{
			tools::no_core_dump();
			return;
		}

		lpath = std::format("{}/{}", lpath, tools::em<ELOG_TYPE>::tolower_name(m_config.m_type));
		if (create_dir(lpath) == false)
		{
			tools::no_core_dump();
			return;
		}

		const int32_t lnow = static_cast<int32_t>(tools::time::gettime());
		std::string ltimestr = tools::time2str(lnow, "%Y-%m-%d");
		lpath = std::format("{}/{}", lpath, ltimestr);
		if (create_dir(lpath) == false)
		{
			tools::no_core_dump();
			return;
		}

		std::cout << "log dir:[" << lpath << "]" << std::endl;

		std::string ldaytimestr = tools::time2str(lnow, "%H-%M-%S");
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
		if (tab == nullptr)
		{
			return;
		}
		m_stream << 
			std::format(
				R"(
[{}][name:{},serverid:{},tcount:{}]{}
[
{}
]
)"
				, tools::time::time2str(alog->m_time, "%H:%M:%S")
				, tab->m_name
				, tab->m_id
				, tab->m_tcount
				, alog->m_src
				, alog->m_data
		);
		++m_count;
		if (flush_count())
		{
			// Rotation is size-based in terms of log lines rather than bytes,
			// which keeps file boundaries predictable during debugging.
			close_fstream();
			create();
		}
	}

	// Dedicated logfile variant for BI/event streams.
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
		// BI logs are already structured upstream, so they are written as
		// one-line records without the verbose multiline wrapper.
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
