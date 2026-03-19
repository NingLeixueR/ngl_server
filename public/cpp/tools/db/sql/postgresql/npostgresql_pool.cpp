/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for postgresql.


#include "tools/db/sql/postgresql/npostgresql_pool.h"

namespace ngl
{
	void npostgresql_pool::init(const xarg_db& adbarg)
	{
		if (!m_dbs.empty())
		{
			tools::no_core_dump();
			return;
		}
		const tab_servers* tab = ttab_servers::instance().const_tab();
		if (tab == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		m_dbs.resize(tab->m_threadnum);
		for (int i = 0; i < tab->m_threadnum; ++i)
		{
			m_dbs[i] = std::make_shared<npostgresql>();
			if (!m_dbs[i]->connectdb(adbarg))
			{
				tools::no_core_dump();
				continue;
			}
		}
		return;
	}

	npostgresql* npostgresql_pool::get(int32_t aindex)
	{
		if (aindex < 0 || static_cast<std::size_t>(aindex) >= m_dbs.size())
		{
			tools::no_core_dump();
			return nullptr;
		}
		return m_dbs[aindex] == nullptr ? nullptr : m_dbs[aindex].get();
	}

}// namespace ngl
