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

#include "tools/db/sql/postgresql/npostgresql.h"
#include "actor/tab/ttab_servers.h"
#include "tools/log/nlog.h"

namespace ngl
{
	class npostgresql_pool
	{
		npostgresql_pool(const npostgresql_pool&) = delete;
		npostgresql_pool& operator=(const npostgresql_pool&) = delete;

		std::vector<std::shared_ptr<npostgresql>> m_dbs;

		npostgresql_pool() = default;
	public:
		static npostgresql_pool& instance()
		{
			static npostgresql_pool ltemp;
			return ltemp;
		}
		// Create one DB connection per worker thread using the shared config.
		bool init(const xarg_db& adbarg);

		// Return the connection assigned to the calling/target worker thread.
		npostgresql* get(int32_t aindex);
	};
}// namespace ngl
