#pragma once

#include "db.h"

namespace ngl
{
	class db_pool
	{
		db_pool() = delete;
		db_pool(const db_pool&) = delete;
		db_pool& operator=(const db_pool&) = delete;

		static std::vector<db*> m_vec;
	public:
		// # 初始化db连接池
		static bool init(uint32_t asize, const dbarg& arg);
		// # 根据线程id获取池中对应的连接
		static db* get(uint32_t aindex);
	};
}// namespace ngl
