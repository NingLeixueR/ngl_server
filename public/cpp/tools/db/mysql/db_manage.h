/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "ttab_mergearea.h"
#include "ttab_servers.h"
#include "nserialize.h"
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
				std::cout << "db xml config dbprotobinary falile" << std::endl;
				tools::no_core_dump();
			}
		}

		template <typename T>
		static void serialize(db* adb, bool isbinary, T& adata)
		{
			return adb->m_malloc.serialize<T>(adata, isbinary);
		}

		// ���ݿⱣ�淽ʽ  �����ƻ���Json��ʽ
		template <typename T>
		static bool unserialize(db* adb, bool isbinary, T& adata, const char* abuff, int alen)
		{
			if (isbinary)
			{
				ngl::ser::serialize_pop lserialize(abuff, alen);
				return ngl::ser::nserialize::pop(&lserialize, adata);
			}
			else
			{
				if constexpr (is_protobuf_message<T>::value)
				{
					return tools::json2proto(abuff, adata);
				}
				else
				{
					return tools::json2custom(abuff, adata);
				}
			}
		}
		
		template <typename T>
		static void save(db* adb, T* adata)
		{
			serialize<T>(adb, m_dbprotobinary, *adata);

			MYSQL_BIND lbind[1];
			memset(lbind, 0, sizeof(MYSQL_BIND));
			lbind[0].buffer_type	= MYSQL_TYPE_LONG_BLOB;
			lbind[0].buffer			= (void*)adb->m_malloc.buff();
			lbind[0].buffer_length	= adb->m_malloc.pos();

			i16_area larea = nguid::area(adata->mid());
			if (larea == 0)
			{
				larea = ttab_servers::instance().tab()->m_area;
			}

			// # INSERT INTO %s  (id,data)VALUES(%lld,'%s')  ON DUPLICATE KEY UPDATE %s
			// # REPLACE INTO �����ɾ�����ݣ�Ȼ���ٲ��롣
			char lbuff[4096] = { 0 };
			int llen = snprintf(
				lbuff, 4096
				, "INSERT INTO %s (id, area, data)VALUES(%lld, %d, ?)  ON DUPLICATE KEY UPDATE data=values(data), area=values(area);"
				, tools::type_name<T>().c_str(), adata->mid(), larea
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
			T* ldata = db_data<T>::find(aid);
			if (ldata == nullptr)
			{
				log_error()->print("db_manage::save fail id:{} !!! name:{}", aid, tools::type_name<T>());
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
				tools::type_name<T>().c_str(), aid
			);
			if (llen <= 0)
			{
				return;
			}
			adb->query(lbuff, llen);
			log_error()->print("{}", lbuff);
		}

		// # ���ر��������������������кϷ�����
		static const char* where_area()
		{
			static std::string lareastr;
			static std::atomic<bool> linit = true;
			if (linit.exchange(false))
			{
				if (lareastr.empty())
				{
					const std::set<i16_area>* lareaset = ttab_servers::instance().get_area(nconfig::area());
					// # ɾ��С��0��Ԫ��
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
			// # �����ݿ��м���
			char lbuff[1024] = { 0 };
			int llen = snprintf(lbuff,1024,
				"SELECT id,data FROM %s WHERE id = '%lld' AND (%s);",
				tools::type_name<T>().c_str(), aid, where_area()
			);
			if (llen <= 0)
			{
				return false;
			}
			log_error()->print("{}", lbuff);
			return adb->select(lbuff, llen,
				[adb, aid](MYSQL_ROW amysqlrow, unsigned long* alens, int arol, int acol)->bool
				{
					T ldata;
					bool lunserialize = db_manage::unserialize<T>(
						adb, m_dbprotobinary, ldata, amysqlrow[1], alens[1]
					);
					if (lunserialize == false)
					{
						return false;
					}
					ngl::db_data<T>::set(ldata.mid(), ldata);
					return true;
				}
			);
		}

		template <typename T>
		static bool select(db* adb)
		{
			// # �����ݿ��м���
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff, 1024, "SELECT id,data FROM %s WHERE %s;",
				tools::type_name<T>().c_str(), where_area()
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
					if (!unserialize<T>(adb, m_dbprotobinary, ldata, amysqlrow[1], alens[1]))
					{
						return false;
					}
					ngl::db_data<T>::set(ldata.mid(), ldata);
					return true;
				}
			);
		}

		// # ���س�id ��ֹ�ڴ洩͸
		template <typename T>
		static bool select(db* adb, std::set<int64_t>& aidset)
		{
			// # �����ݿ��м���
			char lbuff[1024] = { 0 };
			int llen = snprintf(
				lbuff, 1024, "SELECT id FROM %s WHERE %s;",
				tools::type_name<T>().c_str(), where_area()
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