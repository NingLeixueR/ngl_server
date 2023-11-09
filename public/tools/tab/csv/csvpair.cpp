#include "csvpair.h"

namespace ngl
{
	csvpair::csvpair() :
		m_pos(0),
		m_doublequotationmarks(false),
		m_fg(',')
	{}

	void csvpair::clear()
	{
		m_data.clear();
		m_pos = 0;
		m_doublequotationmarks = false;
	}
}