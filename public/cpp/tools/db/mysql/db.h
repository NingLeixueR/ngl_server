#pragma once
  
#include <iostream> 
#include <cstdint>
#include <sstream> 
#include <vector> 
#include <string>
#include <map>  

#include "include3part.h"
#include "malloc_buff.h"
#include "xmlinfo.h"

#define _SQL_BUFF_SIZE_ (102400)
#define _SQL_BUFF_COUNT_ (10)

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

		// # stmt 相关
		bool stmt_query(const char* asql, int alen, MYSQL_BIND* abind);

		malloc_buff m_malloc;
	private:
	};
}// namespace ngl
