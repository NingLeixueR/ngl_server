#pragma once

#include "ttab_mergearea.h"
#include "serialize.h"
#include "db_data.h"
#include "db_buff.h"
#include "tools.h"
#include "db.h"

namespace ngl
{
	class db_manage
	{ 
		static bool m_dbprotobinary;
	public:
		static void init()
		{
			if (nconfig::get_publicconfig()->find("dbprotobinary", m_dbprotobinary) == false)
			{
				log_error()->print("db xml config dbprotobinary falile");
			}
		}

		template <typename T>
		static void serialize(db* adb, T& adata)
		{
			return adb->m_malloc.serialize(adata);
		}

		template <typename T>
		static bool unserialize(db* adb, T& adata, char* abuff, int alen)
		{
			ngl::unserialize lunser(abuff, alen);
			if (!lunser.pop(adata))
			{
				return false;
			}
			return true;
		}
		
		template <typename T>
		static void save(db* adb, T& adata)
		{
			static protobuf_data<T> m_savetemp;
			if (m_savetemp.m_data == nullptr)
			{
				m_savetemp.make();
				m_savetemp.m_isbinary = m_dbprotobinary;
			}
			*m_savetemp.m_data = adata;

			db_manage::serialize(adb, m_savetemp);

			MYSQL_BIND lbind[1];
			memset(lbind, 0, sizeof(MYSQL_BIND));
			lbind[0].buffer_type	= MYSQL_TYPE_LONG_BLOB;
			lbind[0].buffer			= (void*)adb->m_malloc.buff();
			lbind[0].buffer_length	= adb->m_malloc.pos();

			i16_area larea = nguid::area(adata.m_id());
			if (larea == 0)
			{
				larea = ttab_servers::instance().tab()->m_area;
			}

			// # INSERT INTO %s  (id,data)VALUES(%lld,'%s')  ON DUPLICATE KEY UPDATE %s
			// # REPLACE INTO 则会先删除数据，然后再插入。
			char lbuff[4096] = { 0 };
			int llen = snprintf(
				lbuff, 4096
				, "INSERT INTO %s (id, area, data)VALUES(%lld, %d, ?)  ON DUPLICATE KEY UPDATE data=values(data), area=values(area);"
				, tools::protobuf_tabname<T>::name().c_str(), adata.m_id(), larea
			);

			if (llen <= 0)
			{
				return;
			}
			adb->stmt_query(lbuff, llen, lbind);
			adb->m_malloc.reset();
			log_error()->print("{}", lbuff);
		}

		template <typename T>
		static void save(db* adb, i64_actorid aid)
		{
			T ldata;
			if (db_data<T>::get(aid, ldata) == false)
			{
				log_error()->print("db_manage::save fail id:{} !!! name:{}", aid, tools::protobuf_tabname<T>::name());
				return;
			}
			save(adb, ldata);
		}

		template <typename T>
		static void save(db* adb)
		{
			db_data<T>::foreach([adb](T& adata)
				{
					db_manage::save(adb, adata);
				});
		}

		template <typename T>
		static void del(db* adb, i64_actorid aid)
		{
			char lbuff[1024] = { 0 };
			int llen = snprintf(lbuff,1024,
				"DELETE FROM %s WHERE id='%lld';",
				tools::protobuf_tabname<T>::name().c_str(), aid
			);
			if (llen <= 0)
			{
				return;
			}
			adb->query(lbuff, llen);
			log_error()->print("{}", lbuff);
		}

		// # 加载本地配置区服关联的所有合服数据
		static const char* where_area()
		{
			static std::string lareastr;
			static std::atomic<bool> linit = true;
			if (linit.exchange(false))
			{
				if (lareastr.empty())
				{
					const std::set<i16_area>* lareaset = ttab_servers::instance().get_area(nconfig::area());
					// # 删除小于0的元素
					//auto it = lareaset.lower_bound(0);
					//lareaset.erase(lareaset.begin(), it);
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
		static bool select(db* adb, i64_actorid aid)
		{
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(lbuff,1024,
				"SELECT id,data FROM %s WHERE id = '%lld' AND (%s);",
				tools::protobuf_tabname<T>::name().c_str(), aid, where_area()
			);
			if (llen <= 0)
			{
				return false;
			}
			log_error()->print("{}", lbuff);
			return adb->select(lbuff, llen,
				[adb, aid](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
				{
					protobuf_data<T> ldata;
					ldata.m_isbinary = m_dbprotobinary;
					bool lunserialize = db_manage::unserialize(
						adb, ldata, amysqlrow[1], alens[1]
					);
					if (lunserialize == false)
					{
						return false;
					}
					ngl::db_data<T>::set(ldata.m_data->m_id(), *ldata.m_data);
					return true;
				}
			);
		}

		template <typename T>
		static bool select(db* adb)
		{
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff, 1024, "SELECT id,data FROM %s WHERE %s;",
				tools::protobuf_tabname<T>::name().c_str(), where_area()
			);
			if (llen <= 0)
			{
				return false;
			}
			log_error()->print("{}", lbuff);
			return adb->select(lbuff, llen,
				[adb](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
				{
					protobuf_data<T> ldata;
					ldata.m_isbinary = m_dbprotobinary;
					bool lunserialize = ngl::db_manage::unserialize(
						adb, ldata, amysqlrow[1], alens[1]
					);
					if (lunserialize == false)
					{
						return false;
					}
					ngl::db_data<T>::set(ldata.m_data->m_id(), *ldata.m_data);
					return true;
				}
			);
		}

		// # 加载出id 防止内存穿透
		template <typename T>
		static bool select(db* adb, std::set<int64_t>& aidset)
		{
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff, 1024, "SELECT id FROM %s WHERE %s;",
				tools::protobuf_tabname<T>::name().c_str(), where_area()
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