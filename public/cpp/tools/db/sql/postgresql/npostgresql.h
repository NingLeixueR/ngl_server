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
// File overview: Declares interfaces for postgresql.


#pragma once

#include "tools/tab/xml/xmlinfo.h"
#include "tools/db/sql/db_buff.h"

#include <libpq-fe.h>

namespace ngl
{
	class npostgresql
	{
		npostgresql(const npostgresql&) = delete;
		npostgresql& operator=(const npostgresql&) = delete;

		bool	m_connectdb = false;
		PGconn* m_postgresql = nullptr;
		xarg_db	m_arg;
	public:
		npostgresql() = default;

		PGconn* postgresql()
		{
			return m_postgresql;
		}

		// # Connectiondatabase
		bool connectdb(const xarg_db& arg);

		// # Closedatabase
		void closedb();

		// # Execute SQL statements
		bool query(const char* asql);

		// # Callbackin data
		using callback = std::function<bool(PGresult*)>;
		bool select(const char* asql, int resultFormat, const callback& aback);

		db_buff m_malloc;
	};


}//namespace ngl