#pragma once

#include <vector>
#include <string>
#include <map>

#include "actor_role.h"
#include "tools.h"
#include "nlog.h"

namespace ngl
{
	class consume
	{
	public:
		static bool get(int atid, int acount, std::map<int32_t, int32_t>& amap);
		static bool check(actor_role* arole, std::map<int32_t, int32_t>& amap);
		static bool check(actor_role* arole, int atid, int acount);
		static bool use(actor_role* arole, int atid, int acount);
	};
}// namespace ngl
