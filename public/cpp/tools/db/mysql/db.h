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

#include <mysql.h>
#include <iostream> 
#include <cstdint>
#include <sstream>
#include <vector>
#include <string>
#include <map>  

#include "db_buff.h"
#include "xmlinfo.h"

namespace ngl
{
	class db
	{
		db(const db&) = delete;
		db& operator=(const db&) = delete;

		bool	m_connectdb;
		MYSQL*	m_mysql;
		xarg_db	m_arg;
	public:
		db();

		// # 连接数据库
		bool connectdb(const xarg_db& arg);

		// # 关闭数据库
		void closedb();

		// # 选择数据库
		bool changedb(MYSQL* amysql, std::string& adbname);

		// # 执行sql语句
		bool query(const char* asql, int alen);

		// # 转义 SQL 语句中使用的字符串中的特殊字符
		void escape(const char* asql, int asqllen, std::string& aoutsql);

		// # 在callback中转化数据
		using callback = std::function<bool(MYSQL_ROW, unsigned long*, my_ulonglong, my_ulonglong)>;
		bool select(const char* asql, int asqllen, const callback& aback);

		// # stmt 相关
		bool stmt_query(const char* asql, int alen, MYSQL_BIND* abind);

		db_buff m_malloc;
	};
}// namespace ngl