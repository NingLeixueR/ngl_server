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
#include "grids.h"

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