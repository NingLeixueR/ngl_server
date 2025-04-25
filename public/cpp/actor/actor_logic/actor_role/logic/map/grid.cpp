#include "grid.h"

namespace ngl
{
	bool grid::enter(unit* aunit)
	{
		return m_unitlist.insert(aunit->id()).second;
	}

	void grid::leave(unit* aunit)
	{
		m_unitlist.erase(aunit->id());
	}

	std::set<i64_actorid>* grid::get_unitlist()
	{
		return &m_unitlist;
	}
}