#pragma once
  
#include <iostream> 
#include <cstdint>
#include <sstream> 
#include <vector> 
#include <string>
#include <map>  

#include "include3part.h"
#include "xmlinfo.h"


#define _SQL_BUFF_SIZE_ (102400)

#define _SQL_BUFF_LEN_INIT_ (10)

namespace ngl
{
	class db
	{
		db(const db&) = delete;
		db& operator=(const db&) = delete;

		bool	m_connectdb;
		MYSQL*	m_mysql;
		dbarg	m_arg;

	public:
		db();

		// # 连接数据库
		bool connectdb(const dbarg& arg);

		// # 关闭数据库
		void closedb();

		// # 选择数据库
		bool changedb(MYSQL* amysql, std::string& adbname);

		// # 执行sql语句
		bool query(const char* asql, int alen);

		// # 转义 SQL 语句中使用的字符串中的特殊字符
		void escape(const char* asql, std::string& aoutsql);

		// # 在callback中转化数据
		using callback = std::function<bool(MYSQL_ROW, unsigned long*, uint32_t, uint32_t)>;
		bool select(const char* asql, int asqllen, callback aback);

		class ptr
		{
			db* m_db;
		public:
			char* m_buff;
			int32_t m_bufflen;

			ptr() :
				m_buff(nullptr),
				m_bufflen(0),
				m_db(nullptr)
			{}

			ptr(db* adb) :
				m_buff(nullptr),
				m_bufflen(0),
				m_db(adb)
			{}

			ptr(db* adb, char* abuff, int32_t abufflen) :
				m_buff(abuff),
				m_bufflen(abufflen),
				m_db(adb)
			{}

			~ptr()
			{
				//if(m_buff != nullptr)
				//	m_db->free_buff(*this);
			}
		};

		bool malloc_buff(ptr& aptr, int32_t apos)
		{
			if (_SQL_BUFF_LEN_INIT_ <= apos)
			{
				return false;
			}
			int32_t llen = _SQL_BUFF_SIZE_ * (apos+1);
			for (auto itor = m_bufflist.begin();itor != m_bufflist.end();++itor)
			{
				if (itor->second >= llen)
				{
					aptr.m_buff = itor->first;
					aptr.m_bufflen = itor->second;
					m_bufflist.pop_front();
					return true;
				}
			}
			
			aptr.m_buff = new char[llen];
			aptr.m_bufflen = llen;
			return true;
		}

		void free_buff(ptr& aptr)
		{
			m_bufflist.push_back(std::make_pair(aptr.m_buff, aptr.m_bufflen));
			aptr.m_buff = nullptr;
			aptr.m_bufflen = 0;
		}

		// # stmt 相关
		bool stmt_query(const char* asql, int alen, MYSQL_BIND* abind)
		{
			MYSQL_STMT* lstmt = mysql_stmt_init(m_mysql);
			if (lstmt == nullptr)
				return false;
			int err = 0;
			err = mysql_stmt_prepare(lstmt, asql, alen);
			if (err != 0)
			{
				log_error()->print("mysql_stmt_prepare fail [{}]", err);
				return false;
			}

			err = mysql_stmt_bind_param(lstmt, abind);
			if (err != 0)
			{
				log_error()->print("mysql_stmt_bind_param fail [{}]", err);
				return false;
			}

			err = mysql_stmt_execute(lstmt);
			if (err != 0)
			{
				log_error()->print("mysql_stmt_execute fail [{}]", err);
				return false;
			}
			
			mysql_stmt_close(lstmt);
			return true;
		}
	private:
		std::list<std::pair<char*,int32_t>> m_bufflist;
	};
}// namespace ngl
