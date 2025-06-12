#include "ttab_mergearea.h"

namespace ngl
{
	std::map<i16_area, i16_area> ttab_mergearea::m_merge1;
	std::map<i16_area, std::set<i16_area>> ttab_mergearea::m_merge2;
	bool ttab_mergearea::m_load;
}//namespace ngl