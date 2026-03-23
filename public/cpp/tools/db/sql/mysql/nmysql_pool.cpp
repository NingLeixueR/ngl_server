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
// File overview: Implements logic for mysql.

#include "tools/db/sql/mysql/nmysql_pool.h"

namespace ngl
{
	bool nmysql_pool::init(const xarg_db& adbarg)
	{
		if (!m_dbs.empty())
		{
			log_error()->print("nmysql_pool::init repeated");
			return false;
		}
		const tab_servers* tab = ttab_servers::instance().const_tab();
		if (tab == nullptr)
		{
			log_error()->print("nmysql_pool::init missing tab_servers");
			return false;
		}
		m_dbs.resize(tab->m_threadnum);
		for (int32_t li = 0; li < tab->m_threadnum; ++li)
		{
			m_dbs[li] = std::make_shared<nmysql>();
			if (!m_dbs[li]->connectdb(adbarg))
			{
				log_error()->print("nmysql_pool::init connect fail idx:{}", li);
				m_dbs.clear();
				return false;
			}
		}
		return true;
	}

	nmysql* nmysql_pool::get(int32_t aindex)
	{
		if (aindex < 0 || static_cast<std::size_t>(aindex) >= m_dbs.size())
		{
			log_error()->print("nmysql_pool::get invalid idx:{} size:{}", aindex, m_dbs.size());
			return nullptr;
		}
		if (m_dbs[aindex] == nullptr)
		{
			log_error()->print("nmysql_pool::get null db idx:{}", aindex);
			return nullptr;
		}
		return m_dbs[aindex].get();
	}

}// namespace ngl
