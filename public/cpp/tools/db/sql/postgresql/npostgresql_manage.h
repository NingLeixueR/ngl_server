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

#include "tools/db/sql/postgresql/npostgresql_pool.h"
#include "tools/db/sql/postgresql/npostgresql.h"
#include "tools/serialize/nserialize.h"
#include "actor/tab/ttab_mergearea.h"
#include "tools/db/sql/scope_guard.h"
#include "actor/tab/ttab_servers.h"
#include "tools/db/sql/db_data.h"
#include "tools/db/sql/db_buff.h"
#include "tools/tools.h"

namespace ngl
{
	class npostgresql_manage
	{
		static bool m_dbprotobinary;
	public:
		static bool init()
		{
			xarg_info* linfo = nconfig.info();
			if (linfo == nullptr)
			{
				log_error()->print("npostgresql_manage::init missing public config");
				return false;
			}
			if (!linfo->find("dbprotobinary", m_dbprotobinary))
			{
				log_error()->print("npostgresql_manage::init missing dbprotobinary");
				return false;
			}
			return true;
		}

		template <typename T>
		static bool save(npostgresql* adb, T* adata)
		{
			if (adb == nullptr || adata == nullptr)
			{
				return false;
			}
			scope_guard lfree([adb]()noexcept { adb->m_malloc.reset(); });
			if (!adb->m_malloc.serialize<T>(m_dbprotobinary, *adata))
			{
				return false;
			}

			i16_area larea = nguid::area(adata->mid());
			if (larea == 0)
			{
				larea = ttab_servers::instance().const_tab()->m_area;
			}

			std::string lsql = std::format(
				"INSERT INTO {} (id, area, data) VALUES({}, {}, $1) ON CONFLICT (id) DO UPDATE SET area = EXCLUDED.area, data = EXCLUDED.data"
				, tools::type_name<T>().c_str(), adata->mid(), larea
			);

			// PostgreSQL receives the serialized row as one binary parameter.
			const char* param_values[1];
			int param_lengths[1];
			int param_formats[1] = { 1 }; // 0 = text, 1 = binary.

			// Bind the serialized blob payload.
			param_values[0] = adb->m_malloc.buff();
			param_lengths[0] = adb->m_malloc.pos();

			// Execute the prepared parameterized statement.
			PGresult* res = PQexecParams(
				adb->postgresql(), lsql.c_str(), 1, nullptr, param_values, param_lengths, param_formats, 0
			);
			scope_guard lfreeres([res]()noexcept { PQclear(res); });
			if (PQresultStatus(res) != PGRES_COMMAND_OK)
			{
				log_error()->print("npostgresql::save fail id:{} name:{} error:{}", adata->mid(), tools::type_name<T>(), PQerrorMessage(adb->postgresql()));
				return false;
			}
			log_error()->print(lsql);
			return true;
		}

		template <typename T>
		static bool save(npostgresql* adb, i64_actorid aid)
		{
			T* ldata = db_data<T>::find(aid);
			if (ldata == nullptr)
			{
				log_error()->print("npostgresql_manage::save fail id:{} !!! name:{}", aid, tools::type_name<T>());
				return false;
			}
			return save(adb, ldata);
		}

		template <typename T>
		static bool save(npostgresql* adb)
		{
			db_data<T>::foreach([adb](T& adata)
				{
					npostgresql_manage::save(adb, &adata);
				}
			);
			return true;
		}

		template <typename T>
		static bool del(npostgresql* adb, i64_actorid aid)
		{
			if (adb == nullptr)
			{
				return false;
			}
			std::string lsql = std::format(
				"DELETE FROM {} WHERE id={}", tools::type_name<T>().c_str(), aid
			);
			if (!adb->query(lsql.c_str()))
			{
				return false;
			}
			log_error()->print(lsql);
			return true;
		}

		// Restrict table queries to the local area or merged-area visibility set.
		static const char* where_area()
		{
			static std::string lareastr;
			static std::atomic linit = true;
			if (linit.exchange(false))
			{
				if (lareastr.empty())
				{
					const std::set<i16_area>* lareaset = ttab_servers::instance().get_area(nconfig.area());
					if (lareaset == nullptr || lareaset->empty())
					{
						lareastr = " 1 = 1 ";
					}
					else
					{
						tools::splicing(*lareaset, " OR area = ", lareastr);
						lareastr = " area = " + lareastr;
					}
				}
			}
			return lareastr.c_str();
		}

		template <typename T>
		static bool select(npostgresql* adb, i64_actorid aid)
		{
			if (adb == nullptr)
			{
				return false;
			}
			// Load one concrete row.
			std::string lsql = std::format(
				"SELECT id,data FROM {} WHERE id = {} AND ({})", tools::type_name<T>().c_str(), aid, where_area()
			);
			log_error()->print(lsql);
			return adb->select(lsql.c_str(), 1, [adb, aid](PGresult* result)->bool
				{
					int rows = PQntuples(result);
					for (int i = 0; i < rows; i++)
					{
						T ldata;
						if (!adb->m_malloc.unserialize(m_dbprotobinary, ldata, PQgetvalue(result, i, 1), PQgetlength(result, i, 1)))
						{
							return false;
						}
						ngl::db_data<T>::set(ldata.mid(), ldata);
					}
					return true;
				}
			);
		}

		template <typename T>
		static bool select(npostgresql* adb)
		{
			if (adb == nullptr)
			{
				return false;
			}
			// Load the full visible table.
			std::string lsql = std::format(
				"SELECT id,data FROM {} WHERE {}", tools::type_name<T>().c_str(), where_area()
			);
			log_error()->print(lsql);
			return adb->select(lsql.c_str(), 1, [adb](PGresult* result)->bool
				{
					int rows = PQntuples(result);
					for (int i = 0; i < rows; i++)
					{
						T ldata;
						if (!adb->m_malloc.unserialize(m_dbprotobinary, ldata, PQgetvalue(result, i, 1), PQgetlength(result, i, 1)))
						{
							return false;
						}
						ngl::db_data<T>::set(ldata.mid(), ldata);
					}
					return true;
				}
			);
		}

		// Load only ids so later point-lookups can avoid cache penetration.
		template <typename T>
		static bool select(npostgresql* adb, std::set<int64_t>& aidset)
		{
			if (adb == nullptr)
			{
				return false;
			}
			// Load the visible id set only.
			std::string lsql = std::format(
				"SELECT id FROM {} WHERE {}", tools::type_name<T>().c_str(), where_area()
			);
			log_error()->print(lsql);
			return adb->select(lsql.c_str(), 0, [&aidset](PGresult* result)->bool
				{
					int rows = PQntuples(result);
					for (int i = 0; i < rows; i++)
					{
						aidset.insert(tools::lexical_cast<int64_t>(PQgetvalue(result, i, 0)));
					}
					return true;
				}
			);
		}
	};
}// namespace ngl
