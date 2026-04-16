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
// File overview: Declares interfaces for log items.

#pragma once

#include "tools/serialize/ndefine.h"
#include "tools/tab/xml/sysconfig.h"
#include "tools/tools/tools_time.h"
#include "tools/log/logprintf.h"
#include "tools/tools.h"

#include <source_location>
#include <sstream>
#include <format>
#include <string>
#include <memory>

namespace ngl
{
	struct np_logitem
	{
		int				m_serverid = -1;			// Serverid
		ELOGLEVEL		m_loglevel;					// Loglevel
		ELOG_TYPE		m_type;						// Logtype
		std::string		m_src;						// Log fileposition
		std::string		m_data;						// Logcontent
		int32_t			m_time = -1;				// Log time

		DPROTOCOL(logitem, m_serverid, m_loglevel, m_type, m_src, m_data, m_time)
	};

	struct nactor_logitem
	{
	private:
		std::string				m_src;						// Log fileposition
		ELOG_TYPE				m_logtype;
		std::source_location	m_source;
	public:
		std::stringstream		m_stream;
		ELOGLEVEL				m_level;
		bool					m_isnet;
		static bool				m_init;
	public:
		explicit nactor_logitem(ELOGLEVEL alevel = ELOG_NONE) :
			m_logtype(ELOG_DEFAULT),
			m_level(alevel),
			m_isnet(false)
		{
		}

		explicit nactor_logitem(
			ELOGLEVEL alevel, ELOG_TYPE alogtype, bool aisnet,
			const std::source_location& asource = std::source_location::current()
		) :
			m_logtype(alogtype),
			m_source(asource),
			m_level(alevel),
			m_isnet(aisnet)
		{
		}
	private:
		void set_source()
		{
			std::string_view lname = m_source.file_name();
			// Truncate to "public/..." for readability.
			// Handles both forward and backward slashes from different compilers.
			auto lpos = lname.find("public");
			if (lpos == std::string_view::npos)
			{
				// Fallback: extract just the filename.
				auto lsep = lname.find_last_of("/\\");
				lname = (lsep != std::string_view::npos) ? lname.substr(lsep + 1) : lname;
			}
			else
			{
				lname = lname.substr(lpos);
			}
			m_src = std::format("{}:{}", lname, m_source.line());
		}

		void send_log(std::string&& adata, int32_t atime);
		void send(std::shared_ptr<np_logitem> apro, bool aisnet);
	public:
		static bool check_level(ELOGLEVEL alevel)
		{
			return !(alevel < ngl::sysconfig::logconsolelevel() && alevel < ngl::sysconfig::logwritelevel());
		}

		template <typename ...ARGS>
		void print(const std::format_string<ARGS...>& aformat, const ARGS&... aargs)
		{
			if (!check_level(m_level))
			{
				return;
			}

			std::string ldata = m_stream.str();
			ldata += std::vformat(aformat.get(), std::make_format_args(aargs...));

			int32_t ltime = (int32_t)tools::time::gettime();
			set_source();

			if (m_level >= ngl::sysconfig::logconsolelevel())
			{
				char ltimebuff[1024];
				tools::time::time2str(ltimebuff, 1024, ltime, "%Y/%m/%d %H:%M:%S");
				logprintf::printf(m_level, m_src.c_str(), ltimebuff, ldata.c_str());
			}

			if (m_init && m_level >= ngl::sysconfig::logwritelevel())
			{
				send_log(std::move(ldata), ltime);
			}
		}

		void print(const char* astr)
		{
			print("{}", astr);
		}

		void print(const std::string& astr)
		{
			print("{}", astr);
		}

		template <typename T>
		nactor_logitem& operator<<(const T& adata)
		{
			if (m_init && check_level(m_level))
			{
				m_stream << adata;
			}
			return *this;
		}

		nactor_logitem& operator<<(std::ostream& (*amanipulator)(std::ostream&))
		{
			if (m_init && check_level(m_level))
			{
				m_stream << amanipulator;
			}
			return *this;
		}
	};
}//namespace ngl
