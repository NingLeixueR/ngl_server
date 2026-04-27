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
// File overview: MySQL connection wrapper with query execution and result-set iteration.

#pragma once

#include "tools/tab/xml/xmlinfo.h"
#include "tools/db/sql/db_buff.h"

#include <iostream> 
#include <mysql.h>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <vector>
#include <string>
#include <map>  

namespace ngl
{
	class nmysql
	{
		nmysql(const nmysql&) = delete;
		nmysql& operator=(const nmysql&) = delete;

		bool	m_connectdb = false;
		MYSQL*	m_mysql = nullptr;
		xarg_db	m_arg;
	public:
		nmysql() = default;

		// Open one MySQL connection using the supplied XML config.
		bool connectdb(const xarg_db& arg);

		// Close the live MySQL connection if one exists.
		void closedb();

		// Switch the active schema on an existing connection.
		bool changedb(MYSQL* amysql, std::string& adbname);

		// Execute one SQL statement that does not return rows.
		bool query(const char* asql, std::size_t alen);

		// Escape arbitrary bytes before embedding them into SQL text.
		void escape(const char* asql, std::size_t asqllen, std::string& aoutsql);

		// Execute one query and stream rows through the callback.
		using callback = std::function<bool(MYSQL_ROW, unsigned long*, my_ulonglong, my_ulonglong)>;
		bool select(const char* asql, std::size_t asqllen, const callback& aback);

		// Execute one prepared statement with bound parameters.
		bool stmt_query(const char* asql, std::size_t alen, MYSQL_BIND* abind);

		db_buff m_malloc;
	};
}// namespace ngl
