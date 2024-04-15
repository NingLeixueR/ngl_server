#include "ttab_task.h"

namespace ngl
{
	std::map<ETask, int32_t> ttab_task::m_maxval;
	std::map<ETask, std::map<int32_t, ttab_task::receive_complete>> ttab_task::m_map;
}// namespace ngl