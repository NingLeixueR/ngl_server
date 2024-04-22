#pragma once

#include <vector>
#include <string>
#include <map>

#include "manage_csv.h"
#include "splite.h"
#include "tools.h"
#include "nlog.h"

namespace ngl
{
	class actor_role;
	class drop
	{
	public:
		static void init();
		static bool weight(int aid, std::map<int, int>& amap);
		static bool droplist(int aid, int acount, std::map<int, int>& amap);
		static bool droplist(int aid, int acount, google::protobuf::Map<int32_t, int32_t>& amap);
		static bool use(actor_role* arole, int aid, int acount);
	};
}// namespace ngl

