#include "ttab_dbload.h"

namespace ngl
{
	std::map<std::string, tab_dbload*> ttab_dbload::m_name2data;
	tab_dbload ttab_dbload::m_universalize;
}// namespace ngl