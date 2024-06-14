#pragma once

#include "serialize.h"
#include "db_data.h"
#include "tools.h"
#include "db.h"

// 如果此值为true 将把"proto的二进制"转换为十六进制字符串并保存
// 如果此值为false 将把"proto的json串"保存
#define DDBSAVE_PROTO_BINARY (true)

namespace ngl
{
	template <typename T>
	class protobuf_tabname
	{
		static std::string m_dbtabname;
	public:
		static std::string& tabname()
		{
			if (m_dbtabname.empty())
			{
				m_dbtabname = T().descriptor()->full_name();
				ngl::tools::replace("pbdb.", "", m_dbtabname, m_dbtabname);
			}
			return m_dbtabname;
		}
	};

	template <typename T>
	std::string protobuf_tabname<T>::m_dbtabname;

	class db_manage
	{ 
	public:
		template <bool PROTO_BINARY, typename T>
		static int32_t serialize(db* adb, db::ptr& aptr, T& adata)
		{
			int32_t lpos = 0;
			while(lpos < 10)
			{
				adb->malloc_buff(aptr, lpos);
				ngl::serialize lserialize(aptr.m_buff, aptr.m_bufflen);
				if (lserialize.push(adata))
				{
					return lserialize.byte();
				}
				else
				{
					++lpos;
					adb->free_buff(aptr);
					continue;
				}
			}
			throw std::runtime_error("db_manage::serialize");
		}

		template <bool PROTO_BINARY, typename T>
		static bool unserialize(db* adb, T& adata, char* abuff, int alen)
		{
			ngl::unserialize lunser(abuff, alen);
			if (!lunser.pop(adata))
				return false;
			return true;
		}

		template <typename T>
		class save
		{
			static protobuf_data<T> m_savetemp;
		public:
			static void fun(db* adb, T& adata)
			{
				if (m_savetemp.m_data == nullptr)
				{
					m_savetemp.make();
					m_savetemp.m_isbinary = DDBSAVE_PROTO_BINARY;
				}
				*m_savetemp.m_data = adata;

				db::ptr lbinptr;
				int32_t lbuffpos = ngl::db_manage::serialize<DDBSAVE_PROTO_BINARY>(adb, lbinptr, m_savetemp);

				MYSQL_BIND lbind[1];
				memset(lbind, 0, sizeof(MYSQL_BIND));
				lbind[0].buffer_type = MYSQL_TYPE_LONG_BLOB;
				lbind[0].buffer = (void*)lbinptr.m_buff;
				lbind[0].buffer_length = lbuffpos;

				// # INSERT INTO %s  (id,data)VALUES(%lld,'%s')  ON DUPLICATE KEY UPDATE %s
				// # REPLACE INTO 则会先删除数据，然后再插入。
				char lbuff[1024] = { 0 };
				int llen = snprintf(
					lbuff
					, 1024
					, "INSERT INTO %s (id,data)VALUES(%lld,?)  ON DUPLICATE KEY UPDATE data=values(data);"
					, protobuf_tabname<T>::tabname().c_str()
					, adata.m_id()
				);

				if (llen <= 0)
					return;
				adb->stmt_query(lbuff, llen, lbind);
				adb->free_buff(lbinptr);
			}

			static void fun(db* adb, i64_actorid aid)
			{
				T ldata;
				if (db_data<T>::get(aid, ldata) == false)
				{
					log_error()->print(
						"save by id[{}] !!![{}]",
						aid,
						protobuf_tabname<T>::tabname()
					);
					return;
				}
				fun(adb, ldata);
			}

			static void fun(db* adb)
			{
				db_data<T>::foreach([adb](T& adata)
					{
						save<T>::fun(adb, adata);
					});
			}
		};

		template <typename T>
		class del
		{
		public:
			static void fun(db* adb, i64_actorid aid)
			{
				char lbuff[1024] = { 0 };				
				int llen = snprintf(
					lbuff, 
					1024, 
					"DELETE FROM %s WHERE id='%lld';",
					protobuf_tabname<T>::tabname().c_str(), 
					aid
				);
				if (llen <= 0)
					return;
				adb->query(lbuff, llen);
			}
		};

		template <typename T>
		class select
		{
		public:
			static bool fun(db* adb, i64_actorid aid)
			{
				// # 从数据库中加载
				char lbuff[1024] = { 0 };
				int llen = snprintf(
					lbuff, 
					1024, 
					"SELECT id,data FROM %s WHERE id = '%lld';", 
					protobuf_tabname<T>::tabname().c_str(), 
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
						bool lunserialize = ngl::db_manage::unserialize<DDBSAVE_PROTO_BINARY>(
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

			static bool fun(db* adb)
			{
				// # 从数据库中加载
				char lbuff[1024] = { 0 };
				int llen = snprintf(
					lbuff, 
					1024, 
					"SELECT id,data FROM %s;", 
					protobuf_tabname<T>::tabname().c_str()
				);
				if (llen <= 0)
					return false;
				return adb->select(
					lbuff, llen,
					[adb](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
					{
						protobuf_data<T> ldata;
						ldata.m_isbinary = DDBSAVE_PROTO_BINARY;
						bool lunserialize = ngl::db_manage::unserialize<DDBSAVE_PROTO_BINARY>(
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
			static bool fun(db* adb, std::set<int64_t>& aidset)
			{
				// # 从数据库中加载
				char lbuff[1024] = { 0 };
				int llen = snprintf(
					lbuff, 
					1024, 
					"SELECT id FROM %s;", 
					protobuf_tabname<T>::tabname().c_str()
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
	};

	template <typename T>
	protobuf_data<T> db_manage::save<T>::m_savetemp;
}// namespace ngl
