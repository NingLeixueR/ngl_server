#pragma once

#include "manage_csv.h"

namespace ngl
{
	class actor_role;
	// 物品合成(物品转换,分解)
	class synthesis
	{
	public:
		static bool use(actor_role* arole, int32_t aid);
	};
}// namespace ngl