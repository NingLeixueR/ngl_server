#pragma once
  
#include <iostream> 
#include <cstdint>
#include <sstream> 
#include <vector> 
#include <string>
#include <map>  

#include "include3part.h"
#include "xmlinfo.h"

namespace ngl
{
	class db
	{
		bool	m_connectdb;
		MYSQL*	m_mysql;
		dbarg	m_arg;
	public:
		db();
		//// --- 连接数据库
		bool connectdb(const dbarg& arg);
		void closedb();
		//// --- 选择数据库
		bool changedb(MYSQL* amysql, std::string& adbname);
		//// --- 执行sql语句
		bool query(const char* asql, int alen);
		void escape(const char* asql, std::string& aoutsql);
		//// --- 在callback中转化数据
		using callback = std::function<bool(MYSQL_ROW, unsigned long*, uint32_t, uint32_t)>;
		bool select(const char* asql, int asqllen, callback aback);

		char*	m_buff1;
		char*	m_buff2;
		char*	m_buffsql;
		int		m_bufflen1;
		int		m_bufflen2;
		int		m_buffsqllen;
	};
}// namespace ngl
