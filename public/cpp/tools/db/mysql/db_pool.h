#pragma once

#include "db.h"

namespace ngl
{
	class db_pool
	{
		db_pool(const db_pool&) = delete;
		db_pool& operator=(const db_pool&) = delete;

		std::vector<db*> m_vec;

		db_pool()
		{}
	public:
		static db_pool& instance()
		{
			static db_pool ltemp;
			return ltemp;
		}
		// # 初始化db连接池:目前连接池 只支持单配置，
		// # 这意味着单个进程只能由一个数据库连接配置
		void init(const dbarg& adbarg);

		// # 根据线程id获取池中对应的连接
		db* get(int32_t aindex);
	};
}// namespace ngl
