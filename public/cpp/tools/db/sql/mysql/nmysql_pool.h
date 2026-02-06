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

#include "ttab_servers.h"
#include "nmysql.h"
#include "nlog.h"

namespace ngl
{
	class nmysql_pool
	{
		nmysql_pool(const nmysql_pool&) = delete;
		nmysql_pool& operator=(const nmysql_pool&) = delete;

		std::vector<std::shared_ptr<nmysql>> m_dbs;

		nmysql_pool() = default;
	public:
		static nmysql_pool& instance()
		{
			static nmysql_pool ltemp;
			return ltemp;
		}
		// # 初始化db连接池:目前连接池 只支持单配置，
		// # 这意味着单个进程只能由一个数据库连接配置
		void init(const xarg_db& adbarg);

		// # 根据线程id获取池中对应的连接
		nmysql* get(int32_t aindex);
	};
}// namespace ngl