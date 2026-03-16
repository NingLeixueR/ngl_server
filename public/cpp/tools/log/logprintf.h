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
// File overview: Declares interfaces for log.

#pragma once

#include "tools/tab/xml/sysconfig.h"

#include <fstream>
#include <memory>

namespace ngl
{
	enum ELOG_TYPE
	{
		ELOG_DEFAULT	= 0x00000001,
		ELOG_BI			= 0x00000002,
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
			enum
			{
				min_flush_time = 1000,
			};

			i32_actordataid m_id;					// nlogactor
			std::string		m_dir;					// Directory
			ELOG_TYPE		m_type = ELOG_DEFAULT;	// Logtype
			int32_t			m_flush_time = 0;		// Writefile

			int32_t flush()
			{
				return std::max(m_flush_time, (int32_t)min_flush_time);
			}
		};
		std::ofstream	m_stream;
		config			m_config;		// Logconfig
		int				m_count = 0;	// Translated comment.
		int				m_fcount = 0;	// Translated comment.

		logfile(const config& aconfig);

		// Translated comment.
		bool flush_count()const;

		// # Closefile
		void close_fstream();

		// # Createdirectory
		bool create_dir(const std::string& apath);

		// # Create
		void create();

		// # Log
		virtual void printf(const np_logitem* alog) = 0;

		// # Tofile
		void flush();

		// # Createloginstance
		static std::shared_ptr<logfile> create_make(const config& aconfig);
	};
}// namespace ngl