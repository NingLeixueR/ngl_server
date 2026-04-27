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
// File overview: MySQL connection pool with automatic checkout and health checks.

#pragma once

#include "tools/db/sql/mysql/nmysql.h"
#include "actor/tab/ttab_servers.h"
#include "tools/log/nlog.h"

namespace ngl
{
	class nmysql_pool
	{
		nmysql_pool(const nmysql_pool&) = delete;
		nmysql_pool& operator=(const nmysql_pool&) = delete;

		std::vector<std::shared_ptr<nmysql>> m_dbs;

		nmysql_pool() = default;
	public:
		static nmysql_pool& instance()
		{
			static nmysql_pool ltemp;
			return ltemp;
		}
		// Create one DB connection per worker thread using the shared config.
		bool init(const xarg_db& adbarg);

		// Return the connection assigned to the calling/target worker thread.
		nmysql* get(int32_t aindex);
	};
}// namespace ngl
