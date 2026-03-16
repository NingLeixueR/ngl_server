/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for bag.

#pragma once

#include "actor/actor_logic/actor_role/actor_role.h"
#include "tools/log/nlog.h"
#include "tools/tools.h"

#include <vector>
#include <string>
#include <map>

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