#pragma once

#include "serialize.h"
#include "hexbytes.h"
#include "regular.h"
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
				ngl::regular::replace("pbdb.", "", m_dbtabname, m_dbtabname);
			}
			return m_dbtabname;
		}
	};

	template <typename T>
	std::string protobuf_tabname<T>::m_dbtabname;

	class db_manage
	{ 
	public:
		template <typename T>
		static char* serialize(db* adb, T& adata, bool aistohex = true)
		{
			ngl::serialize lserialize(adb->m_buff1, adb->m_bufflen1);
			if (lserialize.push(adata))
			{
				if (aistohex)
				{
					int lpos = hexbytes::to_hex(adb->m_buff1, lserialize.byte(), adb->m_buff2);
					adb->m_buff2[lpos] = '\0';
					return adb->m_buff2;
				}
				else
				{
					std::swap(adb->m_buff1, adb->m_buff2);
					return adb->m_buff2;
				}
			}
			return nullptr;
		}

		template <typename T>
		static bool unserialize(db* adb, T& adata, char* abuff, int alen, bool aistohex = true)
		{
			if (aistohex)
			{
				int lbufflen = 0;
				if (hexbytes::to_bytes(abuff, alen, adb->m_buff2, lbufflen) == false)
				{
					LogLocalError("db_manage::unserialize(%, %)", dtype_name(T), abuff);
					return false;
				}
					
				ngl::unserialize lunser(adb->m_buff2, lbufflen);
				if (!lunser.pop(adata))
					return false;
			}
			else
			{
				ngl::unserialize lunser(abuff, alen);
				if (!lunser.pop(adata))
					return false;
			}
			return true;
		}

		template <EPROTOCOL_TYPE PROTYPE, typename T>
		class save
		{
		public:
		};

		template <typename T>
		class save<EPROTOCOL_TYPE_CUSTOM, T>
		{
		public:
			static void fun(db* adb, T& adata)
			{
				//char lbuff[] =
				//	"INSERT INTO %s  (%s)VALUES(%s)  ON DUPLICATE KEY UPDATE %s;";
				//REPLACE INTO 则会先删除数据，然后再插入。
				char lbuff[] =
					"REPLACE INTO %s (%s)VALUES(%s);";
				int llen = snprintf(
					adb->m_buff1
					, adb->m_bufflen1
					, lbuff
					, T::name()
					, T::format_parm()
					, T::format_value(adb, adata)
				);
				if (llen <= 0)
					return;
				adb->query(adb->m_buff1, llen);
			}

			static void fun(db* adb, i64_actorid aid)
			{
				T ldata;
				if (dbdata<T>::get(aid, ldata) == false)
				{
					LogLocalError("save by id[%] !!![%]", aid, T::name())
					return;
				}
				fun(adb, ldata);
			}

			static void fun(db* adb)
			{
				for (auto&& [_, data] : db_data<T>::m_data)
					fun(adb, data);
			}
		};

		template <typename T>
		class save<EPROTOCOL_TYPE_PROTOCOLBUFF, T>
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

				char* lsql = ngl::db_manage::serialize(adb, m_savetemp, DDBSAVE_PROTO_BINARY ? true : false);
				if (lsql == nullptr)
					return;

				//REPLACE INTO 则会先删除数据，然后再插入。
				char lbuff[] =
					"REPLACE INTO %s (id,data)VALUES(%lld,'%s');";
				int llen = snprintf(
					adb->m_buff1
					, adb->m_bufflen1
					, lbuff
					, protobuf_tabname<T>::tabname().c_str()
					, adata.m_id()
					, lsql
				);

				if (llen <= 0)
					return;
				adb->query(adb->m_buff1, llen);
			}

			static void fun(db* adb, i64_actorid aid)
			{
				T ldata;
				if (dbdata<T>::get(aid, ldata) == false)
				{
					LogLocalError("save by id[%] !!![%]", aid, protobuf_tabname<T>::tabname())
					return;
				}
				fun(adb, ldata);
			}

			static void fun(db* adb)
			{
				for (auto&& [_, data] : db_data<T>::m_data)
					save(adb, data);
			}
		};

		template <EPROTOCOL_TYPE PROTYPE, typename T>
		class del
		{
		public:
		};

		template <typename T>
		class del<EPROTOCOL_TYPE_PROTOCOLBUFF, T>
		{
		public:
			static void fun(db* adb, i64_actorid aid)
			{
				char lbuff[] = "DELETE FROM %s WHERE id='%lld';";
				int llen = snprintf(adb->m_buff1, adb->m_bufflen1, lbuff, protobuf_tabname<T>::tabname().c_str(), aid);
				if (llen <= 0)
					return;
				adb->query(adb->m_buff1, llen);
			}

			static void fun(db* adb, std::vector<i64_actorid>& aid)
			{
				if (aid.empty())
					return;
				std::string lwhere = "";
				for (int i = 0; i < aid.size(); ++i)
				{
					lwhere += i != 0 ? " OR id = " : " id = ";
					lwhere += boost::lexical_cast<std::string>(aid[i]);
				}
				char lbuff[] = "DELETE FROM %s WHERE %s;";
				int llen = snprintf(adb->m_buff1, adb->m_bufflen1, lbuff, protobuf_tabname<T>::tabname().c_str(), lwhere.c_str());
				if (llen <= 0)
					return;
				adb->query(adb->m_buff1, llen);
			}
		};

		template <typename T>
		class del<EPROTOCOL_TYPE_CUSTOM, T>
		{
		public:
			static void fun(db* adb, i64_actorid aid)
			{
				char lbuff[] = "DELETE FROM %s WHERE id='%lld';";
				int llen = snprintf(adb->m_buff1, adb->m_bufflen1, lbuff, T::name(), aid);
				if (llen <= 0)
					return;
				adb->query(adb->m_buff1, llen);
			}

			static void fun(db* adb, std::vector<i64_actorid>& aid)
			{
				if (aid.empty())
					return;
				std::string lwhere = "";
				for (int i = 0; i < aid.size(); ++i)
				{
					lwhere += i != 0 ? " OR id = " : " id = ";
					lwhere += boost::lexical_cast<std::string>(aid[i]);
				}
				char lbuff[] = "DELETE FROM %s WHERE %s;";
				int llen = snprintf(adb->m_buff1, adb->m_bufflen1, lbuff, T::name(), lwhere.c_str());
				if (llen <= 0)
					return;
				adb->query(adb->m_buff1, llen);
			}
		};

		template <EPROTOCOL_TYPE PROTYPE, typename T>
		class select
		{
		public:
		};

		template <typename T>
		class select<EPROTOCOL_TYPE_CUSTOM, T>
		{
		public:
			static bool fun(db* adb, i64_actorid aid)
			{
				//// --- 从数据库中加载
				char lbuff[] =
					"SELECT %s FROM %s WHERE m_id = '%lld';";
				char lbuff2[1024] = { 0 };
				int llen = snprintf(lbuff2, 1024, lbuff, T::format_parm(), T::name(), aid);
				if (llen <= 0)
					return false;
				return adb->select(
					lbuff2, llen,
					[adb, aid](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
					{
						T ldata;
						if (T::copy2struct(adb, amysqlrow, alens, arol, acol, ldata) == false)
							return false;
						ngl::dbdata<T>::set(ldata.const_mm_id(), ldata);
						return true;
					});
			}

			static bool fun(db* adb)
			{
				//// --- 从数据库中加载
				char lbuff[] =
					"SELECT %s FROM %s;";
				char lbuff2[1024] = { 0 };
				int llen = snprintf(lbuff2, 1024, lbuff, T::format_parm(), T::name());
				if (llen <= 0)
					return false;
				return adb->select(
					lbuff2, llen,
					[adb](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
					{
						T ldata;
				if (T::copy2struct(adb, amysqlrow, alens, arol, acol, ldata) == false)
					return false;
				ngl::dbdata<T>::set(ldata.const_mm_id(), ldata);
				return true;
					});
			}

			// ## 加载出id 防止内存穿透
			static bool fun(db* adb, std::set<int64_t>& aidset)
			{
				//// --- 从数据库中加载
				char lbuff[] =
					"SELECT id FROM %s;";
				char lbuff2[1024] = { 0 };
				int llen = snprintf(lbuff2, 1024, lbuff, T::name());
				if (llen <= 0)
					return false;
				return adb->select(
					lbuff2, llen,
					[adb, &aidset](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
					{
						aidset.insert(boost::lexical_cast<int64_t>(amysqlrow[0]));
						return true;
					});
			}
		};

		template <typename T>
		class select<EPROTOCOL_TYPE_PROTOCOLBUFF, T>
		{
		public:
			static bool fun(db* adb, i64_actorid aid)
			{
				//// --- 从数据库中加载
				char lbuff[] =
					"SELECT id,data FROM %s WHERE id = '%lld';";
				char lbuff2[1024] = { 0 };
				int llen = snprintf(lbuff2, 1024, lbuff, protobuf_tabname<T>::tabname().c_str(), aid);
				if (llen <= 0)
					return false;
				return adb->select(
					lbuff2, llen,
					[adb, aid](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
					{
						protobuf_data<T> ldata;
						ldata.m_isbinary = DDBSAVE_PROTO_BINARY;
						if (ngl::db_manage::unserialize(adb, ldata, amysqlrow[1], alens[1], DDBSAVE_PROTO_BINARY ? true : false) == false)
							return false;
						ngl::dbdata<T>::set(ldata.m_data->m_id(), *ldata.m_data);
						return true;
					});
			}

			static bool fun(db* adb)
			{
				//// --- 从数据库中加载
				char lbuff[] =
					"SELECT id,data FROM %s;";
				char lbuff2[1024] = { 0 };
				int llen = snprintf(lbuff2, 1024, lbuff, protobuf_tabname<T>::tabname().c_str());
				if (llen <= 0)
					return false;
				return adb->select(
					lbuff2, llen,
					[adb](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
					{
						protobuf_data<T> ldata;
						ldata.m_isbinary = DDBSAVE_PROTO_BINARY;
						if (ngl::db_manage::unserialize(adb, ldata, amysqlrow[1], alens[1], DDBSAVE_PROTO_BINARY ? true : false) == false)
							return false;
						ngl::dbdata<T>::set(ldata.m_data->m_id(), *ldata.m_data);
						return true;
					});
			}

			// ## 加载出id 防止内存穿透
			static bool fun(db* adb, std::set<int64_t>& aidset)
			{
				//// --- 从数据库中加载
				char lbuff[] =
					"SELECT id FROM %s;";
				char lbuff2[1024] = { 0 };
				int llen = snprintf(lbuff2, 1024, lbuff, protobuf_tabname<T>::tabname().c_str());
				if (llen <= 0)
					return false;
				return adb->select(
					lbuff2, llen,
					[adb, &aidset](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
					{
						aidset.insert(boost::lexical_cast<int64_t>(amysqlrow[0]));
						return true;
					});
			}
		};
	};

	template <typename T>
	protobuf_data<T> db_manage::save<EPROTOCOL_TYPE_PROTOCOLBUFF, T>::m_savetemp;
}// namespace ngl
