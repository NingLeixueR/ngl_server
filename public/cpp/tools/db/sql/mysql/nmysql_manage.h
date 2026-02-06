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
#pragma once

#include "ttab_mergearea.h"
#include "ttab_servers.h"
#include "scope_guard.h"
#include "nmysql_pool.h"
#include "nserialize.h"
#include "db_data.h"
#include "db_buff.h"
#include "nmysql.h"
#include "tools.h"

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
			// # REPLACE INTO 则会先删除数据，然后再插入。
			char lbuff[4096] = { 0 };
			int llen = snprintf(
				lbuff, 4096
				, "INSERT INTO %s (id, area, data)VALUES(%lld, %d, ?)  ON DUPLICATE KEY UPDATE data=values(data), area=values(area);"
				, tools::type_name<T>().c_str(), adata->mid(), larea
			);

			if (llen <= 0)
			{
				return false;
			}
			adb->stmt_query(lbuff, llen, lbind);
			log_error()->print("{}", lbuff);
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
			char lbuff[1024] = { 0 };
			int llen = snprintf(lbuff,1024,
				"DELETE FROM %s WHERE id='%lld';", tools::type_name<T>().c_str(), aid
			);
			if (llen <= 0)
			{
				return false;
			}
			if (!adb->query(lbuff, llen))
			{
				return false;
			}
			log_error()->print("{}", lbuff);
			return true;
		}

		// # 加载本地配置区服关联的所有合服数据
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
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(lbuff,1024,
				"SELECT id,data FROM %s WHERE id = '%lld' AND (%s);", tools::type_name<T>().c_str(), aid, where_area()
			);
			if (llen <= 0)
			{
				return false;
			}
			log_error()->print("{}", lbuff);
			return adb->select(lbuff, llen, [adb, aid](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
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
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff, 1024, "SELECT id,data FROM %s WHERE %s;", tools::type_name<T>().c_str(), where_area()
			);
			if (llen <= 0)
			{
				return false;
			}
			log_error()->print("{}", lbuff);
			return adb->select(lbuff, llen,
				[adb](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
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

		// # 加载出id 防止内存穿透
		template <typename T>
		static bool select(nmysql* adb, std::set<int64_t>& aidset)
		{
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff, 1024, "SELECT id FROM %s WHERE %s;", tools::type_name<T>().c_str(), where_area()
			);
			if (llen <= 0)
			{
				return false;
			}
			log_error()->print("{}", lbuff);
			return adb->select(lbuff, llen,
				[adb, &aidset](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
				{
					aidset.insert(tools::lexical_cast<int64_t>(amysqlrow[0]));
					return true;
				}
			);
		}
	};
}// namespace ngl