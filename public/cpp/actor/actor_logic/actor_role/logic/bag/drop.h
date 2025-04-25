#pragma once
#include "manage_csv.h"
#include "tools.h"
#include "nlog.h"

#include <vector>
#include <string>
#include <map>

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
		static bool droplist(int aid, int acount, std::map<int, int>& amap1, google::protobuf::Map<int32_t, int32_t>& amap2);
		static bool use(actor_role* arole, int aid, int acount);

		// 其他模块发送奖励
		static bool use(i64_actorid aactorid, int aid, int acount, i64_actorid aactorid2 = nguid::make());
	};
}// namespace ngl

