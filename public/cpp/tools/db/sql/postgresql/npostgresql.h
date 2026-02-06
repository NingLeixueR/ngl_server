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

#include "xmlinfo.h"
#include "db_buff.h"

#include <libpq-fe.h>


namespace ngl
{
	class npostgresql
	{
		npostgresql(const npostgresql&) = delete;
		npostgresql& operator=(const npostgresql&) = delete;

		bool	m_connectdb = false;
		PGconn* m_postgresql = nullptr;
		xarg_db	m_arg;
	public:
		npostgresql() = default;

		PGconn* postgresql()
		{
			return m_postgresql;
		}

		// # 连接数据库
		bool connectdb(const xarg_db& arg);

		// # 关闭数据库
		void closedb();

		// # 执行sql语句
		bool query(const char* asql);

		// # 在callback中转化数据
		using callback = std::function<bool(PGresult*)>;
		bool select(const char* asql, const callback& aback);

		db_buff m_malloc;
	};


}//namespace ngl