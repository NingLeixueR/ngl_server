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

#include "npostgresql_pool.h"
#include "ttab_mergearea.h"
#include "ttab_servers.h"
#include "scope_guard.h"
#include "npostgresql.h"
#include "nserialize.h"
#include "db_data.h"
#include "db_buff.h"
#include "tools.h"

namespace ngl
{
	class npostgresql_manage
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
		static bool save(npostgresql* adb, T* adata)
		{
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

			char lbuff[4096] = { 0 };
			int llen = snprintf(
				lbuff, 4096
				, "INSERT INTO %s (id, area, data) VALUES(%lld, %d, $1) ON CONFLICT (id) DO UPDATE SET area = EXCLUDED.area, data = EXCLUDED.data"
				, tools::type_name<T>().c_str(), adata->mid(), larea
			);

			// 参数准备
			const char* param_values[1];
			int param_lengths[1];
			int param_formats[1] = { 1 }; // 0=text, 1=binary（data 用 binary 格式）

			// data (binary)
			param_values[0] = adb->m_malloc.buff();
			param_lengths[0] = adb->m_malloc.pos();

			// 执行参数化查询
			PGresult* res = PQexecParams(
				adb->postgresql(), lbuff, 1, nullptr, param_values, param_lengths, param_formats, 0 
			);
			scope_guard lfreeres([res]()noexcept { PQclear(res); });
			if (PQresultStatus(res) != PGRES_COMMAND_OK)
			{
				log_error()->print("npostgresql::save fail id:{} name:{} error:{}", adata->mid(), tools::type_name<T>(), PQerrorMessage(adb->postgresql()));
				return false;
			}
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
					npostgresql_manage::save(adb, adata);
				}
			);
			return true;
		}

		template <typename T>
		static bool del(npostgresql* adb, i64_actorid aid)
		{
			char lbuff[1024] = { 0 };
			int llen = snprintf(lbuff, 1024,
				"DELETE FROM %s WHERE id='%lld';", tools::type_name<T>().c_str(), aid
			);
			if (llen <= 0)
			{
				return false;
			}
			if (!adb->query(lbuff))
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
		static bool select(npostgresql* adb, i64_actorid aid)
		{
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(lbuff, 1024,
				"SELECT id,data FROM %s WHERE id = '%lld' AND (%s);", tools::type_name<T>().c_str(), aid, where_area()
			);
			if (llen <= 0)
			{
				return false;
			}
			log_error()->print("{}", lbuff);
			return adb->select(lbuff, [adb, aid](PGresult* result)->bool
				{
					int rows = PQntuples(result);
					int cols = PQnfields(result);
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
			return adb->select(lbuff, [adb](PGresult* result)->bool
				{
					int rows = PQntuples(result);
					int cols = PQnfields(result);
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

		// # 加载出id 防止内存穿透
		template <typename T>
		static bool select(npostgresql* adb, std::set<int64_t>& aidset)
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
			return adb->select(lbuff, [&aidset](PGresult* result)->bool
				{
					int rows = PQntuples(result);
					int cols = PQnfields(result);
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