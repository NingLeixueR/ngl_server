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
// File overview: Individual BI event item with typed payload and timestamp.

#pragma once

#include "actor/protocol/nprotocol.h"
#include "tools/log/bi/bi.h"

namespace ngl
{
	struct bi_item
	{
		i32_serverid	m_serverid = 0;
		i64_actorid		m_roleid = 0;
		int32_t			m_itemid = 0;
		int32_t			m_tid = 0;
		int32_t			m_count = 0;
		int32_t			m_time = 0;
		int32_t			m_adddec = 0; // Positive for gains, negative for consumption.
		std::string		m_mask;

		void print()
		{
			log_bi()->print("item|{}", bi::push(m_serverid, m_roleid, m_itemid, m_tid, m_count, m_time, m_adddec, m_mask));
		}
	};
}//namespace ngl
