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


#include "actor/actor_logic/actor_role/logic/nmap/grids.h"

namespace ngl
{
	void grids::init(int32_t aw, int32_t al, int32_t anx, int32_t any)
	{
		aoi::init(aw, al, anx, any);
		m_grid.resize(grid_count());
	}

	bool grids::enter(i64_actorid aunitid, int32_t ax, int32_t ay)
	{
		return enter(aunitid, id(ax, ay));
	}

	bool grids::enter(i64_actorid aunitid, int32_t agid)
	{
		if (agid >= m_grid.size())
		{
			return false;
		}
		return m_grid[agid].enter(aunitid);
	}

	void grids::leave(i64_actorid aunitid, int32_t ax, int32_t ay)
	{
		leave(aunitid, id(ax, ay));
	}

	void grids::leave(i64_actorid aunitid, int32_t agid)
	{
		if (agid >= m_grid.size())
		{
			return;
		}
		return m_grid[agid].leave(aunitid);
	}

	grid* grids::get_grid(int32_t agid)
	{
		if (agid >= m_grid.size())
		{
			return nullptr;
		}
		return &m_grid[agid];
	}

	grid* grids::get_grid(int32_t ax, int32_t ay)
	{
		uint32_t agid = id(ax, ay);
		if (agid >= m_grid.size())
		{
			return nullptr;
		}
		return &m_grid[agid];
	}
}//namespace ngl