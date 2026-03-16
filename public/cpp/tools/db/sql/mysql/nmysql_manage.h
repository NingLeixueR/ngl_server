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
// File overview: Declares interfaces for mysql.

#pragma once

#include "actor/tab/ttab_mergearea.h"
#include "actor/tab/ttab_servers.h"
#include "tools/scope_guard.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "tools/serialize/nserialize.h"
#include "tools/db/sql/db_data.h"
#include "tools/db/sql/db_buff.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "tools/tools.h"

namespace ngl
{
	class nmysql_manage
	{ 
		static bool m_dbprotobinary;
	public:
		static void init()
		{
			if (nconfig.info()->find("dbprotobinary", m_dbprotobinary) == false)
			{
				std::cout << "db xml config dbprotobinary falile" << std::endl;
				tools::no_core_dump();
			}
		}

		template <typename T>
		static bool save(nmysql* adb, T* adata)
		{
			scope_guard lfree([adb]()noexcept { adb->m_malloc.reset(); });
			if (!adb->m_malloc.serialize<T>(m_dbprotobinary, *adata))
			{
				return false;
			}
			MYSQL_BIND lbind[1];
			memset(lbind, 0, sizeof(MYSQL_BIND));
			lbind[0].buffer_type	= MYSQL_TYPE_LONG_BLOB;
			lbind[0].buffer			= (void*)adb->m_malloc.buff();
			lbind[0].buffer_length	= adb->m_malloc.pos();

			i16_area larea = nguid::area(adata->mid());
			if (larea == 0)
			{
				larea = ttab_servers::instance().const_tab()->m_area;
			}

			// # INSERT INTO %s  (id,data)VALUES(%lld,'%s')  ON DUPLICATE KEY UPDATE %s
			// # REPLACE INTO first deletedata, afterthen.
			std::string lsql = std::format(
				"INSERT INTO {} (id, area, data)VALUES({}, {}, ?)  ON DUPLICATE KEY UPDATE data=values(data), area=values(area);"
				, tools::type_name<T>().c_str(), adata->mid(), larea
			);
			adb->stmt_query(lsql.c_str(), lsql.size(), lbind);
			log_error()->print(lsql);
			return true;
		}

		template <typename T>
		static bool save(nmysql* adb, i64_actorid aid)
		{
			T* ldata = db_data<T>::find(aid);
			if (ldata == nullptr)
			{
				log_error()->print("nmysql_manage::save fail id:{} !!! name:{}", aid, tools::type_name<T>());
				return false;
			}
			return save(adb, ldata);
		}

		template <typename T>
		static bool save(nmysql* adb)
		{
			db_data<T>::foreach([adb](T& adata)
				{
					nmysql_manage::save(adb, adata);
				}
			);
			return true;
		}

		template <typename T>
		static bool del(nmysql* adb, i64_actorid aid)
		{
			std::string lsql = std::format(
				"DELETE FROM {} WHERE id='{}';", tools::type_name<T>().c_str(), aid
			);
			if (!adb->query(lsql.c_str(), lsql.size()))
			{
				return false;
			}
			log_error()->print(lsql);
			return true;
		}

		// # Loadlocalconfigarea all data
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
		static bool select(nmysql* adb, i64_actorid aid)
		{
			// # Fromdatabaseinload
			std::string lsql = std::format(
				"SELECT id,data FROM {} WHERE id = '{}' AND ({});", tools::type_name<T>().c_str(), aid, where_area()
			);
			log_error()->print(lsql);
			return adb->select(lsql.c_str(), lsql.size(), [adb, aid](MYSQL_ROW amysqlrow, unsigned long* alens, my_ulonglong, my_ulonglong)->bool
				{
					T ldata;
					if (!adb->m_malloc.unserialize(m_dbprotobinary, ldata, amysqlrow[1], alens[1]))
					{
						return false;
					}
					ngl::db_data<T>::set(ldata.mid(), ldata);
					return true;
				}
			);
		}

		template <typename T>
		static bool select(nmysql* adb)
		{
			// # Fromdatabaseinload
			std::string lsql = std::format(
				"SELECT id,data FROM {} WHERE {};", tools::type_name<T>().c_str(), where_area()
			);
			log_error()->print(lsql);
			return adb->select(lsql.c_str(), lsql.size(),
				[adb](MYSQL_ROW amysqlrow, unsigned long* alens, my_ulonglong, my_ulonglong)->bool
				{
					T ldata;
					if (!adb->m_malloc.unserialize(m_dbprotobinary, ldata, amysqlrow[1], alens[1]))
					{
						return false;
					}
					ngl::db_data<T>::set(ldata.mid(), ldata);
					return true;
				}
			);
		}

		// # Load id preventcache penetration
		template <typename T>
		static bool select(nmysql* adb, std::set<int64_t>& aidset)
		{
			// # Fromdatabaseinload
			std::string lsql = std::format(
				"SELECT id FROM {} WHERE {};", tools::type_name<T>().c_str(), where_area()
			);
			log_error()->print(lsql);
			return adb->select(lsql.c_str(), lsql.size(),
				[adb, &aidset](MYSQL_ROW amysqlrow, [[maybe_unused]] unsigned long* alens, my_ulonglong, my_ulonglong)->bool
				{
					aidset.insert(tools::lexical_cast<int64_t>(amysqlrow[0]));
					return true;
				}
			);
		}
	};
}// namespace ngl