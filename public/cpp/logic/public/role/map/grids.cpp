#include "grids.h"

namespace ngl
{
	void grids::init(int32_t aw, int32_t al, int32_t anx, int32_t any)
	{
		aoi::init(aw, al, anx, any);
		m_grid.resize(grid_count());
	}

	bool grids::enter(unit* aunit, int32_t ax, int32_t ay)
	{
		return enter(aunit, id(ax, ay));
	}

	bool grids::enter(unit* aunit, int32_t agid)
	{
		if (agid >= m_grid.size())
		{
			return false;
		}
		return m_grid[agid].enter(aunit);
	}

	void grids::leave(unit* aunit, int32_t ax, int32_t ay)
	{
		leave(aunit, id(ax, ay));
	}

	void grids::leave(unit* aunit, int32_t agid)
	{
		if (agid >= m_grid.size())
		{
			return;
		}
		return m_grid[agid].leave(aunit);
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
}