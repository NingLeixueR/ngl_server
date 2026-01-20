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
#include "ttab_servers.h"
#include "db_pool.h"
#include "nlog.h"

namespace ngl
{
	db_pool& db_pool::instance()
	{
		static db_pool ltemp;
		return ltemp;
	}

	void db_pool::init(const xarg_db& adbarg)
	{
		if (!m_dbs.empty())
		{
			tools::no_core_dump();
			return;
		}
		const tab_servers* tab = ttab_servers::instance().tab();
		if (tab == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		m_dbs.resize(tab->m_threadnum);
		for (int i = 0; i < tab->m_threadnum; ++i)
		{
			m_dbs[i] = std::make_shared<db>();
			if (!m_dbs[i]->connectdb(adbarg))
			{
				tools::no_core_dump();
				continue;
			}
		}
		return;
	}

	db* db_pool::get(int32_t aindex)
	{
		if (aindex >= m_dbs.size() && aindex < 0)
		{
			tools::no_core_dump();
			return nullptr;
		}
		return m_dbs[aindex].get();
	}
}// namespace ngl