#pragma once

#include "serialize.h"
#include "db_data.h"
#include "db_buff.h"
#include "tools.h"
#include "db.h"

// 如果此值为true 
// 将"proto的二进制"保存
// 如果此值为false 
// 将把"proto的json串"保存
#define DDBSAVE_PROTO_BINARY (true)

namespace ngl
{

	class db_manage
	{ 
	public:
		template <typename T>
		static int serialize(db* adb, db_buff::ptr& aptr, T& adata)
		{
			return adb->m_malloc.serialize(aptr, adata);
		}

		template <typename T>
		static bool unserialize(db* adb, T& adata, char* abuff, int alen)
		{
			ngl::unserialize lunser(abuff, alen);
			if (!lunser.pop(adata))
				return false;
			return true;
		}
		
		template <typename T>
		static void save(db* adb, T& adata)
		{
			static protobuf_data<T> m_savetemp;
			if (m_savetemp.m_data == nullptr)
			{
				m_savetemp.make();
				m_savetemp.m_isbinary = DDBSAVE_PROTO_BINARY;
			}
			*m_savetemp.m_data = adata;

			db_buff::ptr lbinptr(&adb->m_malloc);
			int32_t lbuffpos = db_manage::serialize(adb, lbinptr, m_savetemp);

			MYSQL_BIND lbind[1];
			memset(lbind, 0, sizeof(MYSQL_BIND));
			lbind[0].buffer_type	= MYSQL_TYPE_LONG_BLOB;
			lbind[0].buffer			= (void*)lbinptr.m_buff;
			lbind[0].buffer_length	= lbuffpos;

			// # INSERT INTO %s  (id,data)VALUES(%lld,'%s')  ON DUPLICATE KEY UPDATE %s
			// # REPLACE INTO 则会先删除数据，然后再插入。
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff
				, 1024
				, "INSERT INTO %s (id,data)VALUES(%lld,?)  ON DUPLICATE KEY UPDATE data=values(data);"
				, tools::protobuf_tabname<T>::tabname().c_str()
				, adata.m_id()
			);

			if (llen <= 0)
				return;
			adb->stmt_query(lbuff, llen, lbind);
			log_error()->print(
				"INSERT INTO {} (id,data)VALUES({},[bindata])"
				, tools::protobuf_tabname<T>::tabname().c_str()
				, adata.m_id()
			);
		}

		template <typename T>
		static void save(db* adb, i64_actorid aid)
		{
			T ldata;
			if (db_data<T>::get(aid, ldata) == false)
			{
				log_error()->print(
					"save by id[{}] !!![{}]",
					aid,
					tools::protobuf_tabname<T>::tabname()
				);
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
			int llen = snprintf(
				lbuff,
				1024,
				"DELETE FROM %s WHERE id='%lld';",
				tools::protobuf_tabname<T>::tabname().c_str(),
				aid
			);
			if (llen <= 0)
				return;
			adb->query(lbuff, llen);
		}

		template <typename T>
		static bool select(db* adb, i64_actorid aid)
		{
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff,
				1024,
				"SELECT id,data FROM %s WHERE id = '%lld';",
				tools::protobuf_tabname<T>::tabname().c_str(),
				aid
			);
			if (llen <= 0)
				return false;
			return adb->select(
				lbuff, llen,
				[adb, aid](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
				{
					protobuf_data<T> ldata;
					ldata.m_isbinary = DDBSAVE_PROTO_BINARY;
					bool lunserialize = db_manage::unserialize(
						adb, ldata, amysqlrow[1], alens[1]
					);
					if (lunserialize == false)
					{
						return false;
					}
					ngl::db_data<T>::set(ldata.m_data->m_id(), *ldata.m_data);
					return true;
				});
		}

		template <typename T>
		static bool select(db* adb)
		{
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff,
				1024,
				"SELECT id,data FROM %s;",
				tools::protobuf_tabname<T>::tabname().c_str()
			);
			if (llen <= 0)
				return false;
			return adb->select(
				lbuff, llen,
				[adb](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
				{
					protobuf_data<T> ldata;
					ldata.m_isbinary = DDBSAVE_PROTO_BINARY;
					bool lunserialize = ngl::db_manage::unserialize(
						adb, ldata, amysqlrow[1], alens[1]
					);
					if (lunserialize == false)
					{
						return false;
					}
					ngl::db_data<T>::set(ldata.m_data->m_id(), *ldata.m_data);
					return true;
				});
		}

		// # 加载出id 防止内存穿透
		template <typename T>
		static bool select(db* adb, std::set<int64_t>& aidset)
		{
			// # 从数据库中加载
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff,
				1024,
				"SELECT id FROM %s;",
				tools::protobuf_tabname<T>::tabname().c_str()
			);
			if (llen <= 0)
				return false;
			return adb->select(
				lbuff, llen,
				[adb, &aidset](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
				{
					aidset.insert(tools::lexical_cast<int64_t>(amysqlrow[0]));
					return true;
				});
		}
	};
}// namespace ngl
