#pragma once

#include "manage_csv.h"

namespace ngl
{
	class actor_role;
	// ��Ʒ�ϳ�(��Ʒת��,�ֽ�)
	class synthesis
	{
	public:
		static bool use(actor_role* arole, int32_t aid);
	};
}// namespace ngl