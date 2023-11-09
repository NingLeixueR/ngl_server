#pragma once

#include <vector>
#include <map>
#include <string>

#include "manage_csv.h"
#include "nlog.h"
#include "tools.h"
#include "splite.h"

namespace ngl
{
	class actor_role;

	class drop
	{
	public:
		static void init()
		{
			tdrop:init();
		}

		static bool weight(int aid, std::map<int, int>& amap);
		static bool droplist(int aid, int acount, std::map<int, int>& amap);
		static bool use(actor_role* arole, int aid, int acount);
	};

}

