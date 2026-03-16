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
// File overview: Implements logic for nmap.


#include "actor/actor_logic/actor_role/logic/nmap/grid.h"

namespace ngl
{
	bool grid::enter(i64_actorid aunitid)
	{
		return m_unitlist.insert(aunitid).second;
	}

	void grid::leave(i64_actorid aunitid)
	{
		m_unitlist.erase(aunitid);
	}

	const std::set<i64_actorid>& grid::get_unitlist()
	{
		return m_unitlist;
	}

	void grid::clear()
	{
		m_unitlist.clear();
	}

	int32_t grid::count()
	{
		return (int32_t)m_unitlist.size();
	}
}//namespace ngl